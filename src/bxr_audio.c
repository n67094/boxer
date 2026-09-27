#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#define STB_VORBIS_IMPLEMENTATION
#include "stb_vorbis.c"

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_audio.h"
#include "bxr_config.h"
#include "bxr_log.h"
#include "bxr_mem.h"
#include "bxr_multimap.h"
#include "bxr_pool.h"

typedef struct bxr_audio_s_
{
  Uint8 *data;
  Uint32 length; // in bytes
  SDL_AudioSpec spec;
} bxr_audio_t_;

typedef struct bxr_voice_s_
{
  bxr_voice_t voice; // pointer to the public voice handle
  SDL_AudioStream *stream;
  int audio_slot; // slot in audios_
  float volume;
  Uint32 position; // in bytes
  bool feeding;    // playing/feeding audio data
  bool one_shot;
  char *tags[BXR_AUDIO_MAX_TAGS]; // tags associated with this voice
  size_t tag_count;
} bxr_voice_t_;

static Uint32 initialized_ = 0;

static bxr_pool_t *audio_pool_ = NULL;
static bxr_audio_t_ *audios_   = NULL;

static bxr_pool_t *voice_pool_ = NULL;
static bxr_voice_t_ *voices_   = NULL;

static SDL_AudioDeviceID device_id_ = 0;
static SDL_AudioSpec device_spec_;

static bxr_multimap_t *audio_tags_ = NULL;

/**
 * ## Internal API
 */

static void
bxr_voice_release_(int voice_slot)
{
  // Remove voice from audio tags if it exists
  bxr_voice_t_ *t = &voices_[voice_slot];
  for (size_t i = 0; i < t->tag_count; ++i) {
    bxr_multimap_remove_value(audio_tags_, t->tags[i], t->voice.id);
    BXR_FREE(t->tags[i]);
  }

  t->audio_slot = -1;
  t->volume     = 1.0f;
  t->position   = 0;
  t->feeding    = false;
  t->one_shot   = false;
  t->tag_count  = 0;
  if (t->stream) {
    SDL_DestroyAudioStream(t->stream);
    t->stream = NULL;
  }

  bxr_pool_release_slot(voice_pool_, voice_slot);
}

static void
bxr_audio_release_(void)
{
  if (device_id_ != 0) {
    SDL_CloseAudioDevice(device_id_);
    device_id_ = 0;
  }

  if (voices_) {
    for (int i = 0; i < BXR_AUDIO_MAX_TRACKS; ++i) {
      if (voices_[i].stream) {
        bxr_voice_release_(i);
      }
    }
    BXR_FREE(voices_);
  }

  if (voice_pool_) {
    bxr_pool_destroy(voice_pool_);
    voice_pool_ = NULL;
  }

  if (audio_tags_) {
    bxr_multimap_destroy(audio_tags_);
    audio_tags_ = NULL;
  }

  if (audios_) {
    for (size_t i = 0; i < BXR_AUDIO_MAX; ++i) {
      if (audios_[i].data) {
        BXR_FREE(audios_[i].data);
      }
    }
    BXR_FREE(audios_);
  }

  if (audio_pool_) {
    bxr_pool_destroy(audio_pool_);
    audio_pool_ = NULL;
  }
}

void
bxr_audio_setup(void)
{
  BXR_ASSERT(initialized_ == 0);

  // Open the default audio device for playback
  device_id_ = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (device_id_ == 0) {
    BXR_LOG_ERROR("Failed to open audio device: %s", SDL_GetError());
    return;
  }

  // Get the audio device format
  if (SDL_GetAudioDeviceFormat(device_id_, &device_spec_, NULL) == false) {
    BXR_LOG_ERROR("Failed to get audio device format: %s", SDL_GetError());
    bxr_audio_release_();
    return;
  }

  // Audio pool for loaded audio slots
  audio_pool_ = bxr_pool_create(BXR_AUDIO_MAX);
  if (!audio_pool_) {
    BXR_LOG_ERROR("Failed to create audio pool");
    bxr_audio_release_();
    return;
  }

  BXR_CALLOC(audios_, BXR_AUDIO_MAX, sizeof(bxr_audio_t_));
  if (!audios_) {
    BXR_LOG_ERROR("Failed to allocate audio slots");
    bxr_audio_release_();
    return;
  }

  // Create a pool for audio voices
  voice_pool_ = bxr_pool_create(BXR_AUDIO_MAX_TRACKS);
  if (!voice_pool_) {
    BXR_LOG_ERROR("Failed to create voice pool");
    bxr_audio_release_();
    return;
  }

  BXR_ALLOC(voices_, sizeof(bxr_voice_t_) * BXR_AUDIO_MAX_TRACKS);
  if (!voices_) {
    BXR_LOG_ERROR("Failed to allocate voice slots");
    bxr_audio_release_();
    return;
  }

  for (int i = 0; i < BXR_AUDIO_MAX_TRACKS; ++i) {
    voices_[i].stream     = NULL;
    voices_[i].audio_slot = -1;
    voices_[i].volume     = 1.0f;
    voices_[i].position   = 0;
    voices_[i].feeding    = false;
    voices_[i].one_shot   = false;
    voices_[i].tag_count  = 0;
  }

  audio_tags_ = bxr_multimap_create(BXR_AUDIO_MAX_TAGS);
  if (!audio_tags_) {
    BXR_LOG_ERROR("Failed to create audio tags multimap");
    bxr_audio_release_();
    return;
  }

  SDL_ResumeAudioDevice(device_id_);

  initialized_ = BXR_INIT_COOKIE;
}

void
bxr_audio_begin_frame(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  for (int i = 0; i < BXR_AUDIO_MAX_TRACKS; ++i) {
    bxr_voice_t_ *voice = &voices_[i];

    // Skip voices that are not active
    if (!voice || !voice->stream) {
      continue;
    }

    // Release finished one-shot voices to free up resources
    if (SDL_GetAudioStreamQueued(voice->stream) == 0) {
      if (voice->one_shot && !voice->feeding) {
        bxr_voice_release_(i);
      }
    }

    // Skip voices that are not feeding or have no stream
    if (!voice->feeding) {
      continue;
    }

    // Get the audio slot for this voice
    int audio_slot = voice->audio_slot;
    if (audio_slot < 0 || audio_slot >= BXR_AUDIO_MAX) {
      BXR_LOG_ERROR("Invalid audio slot for voice %d", i);
      continue;
    }

    // Get the audio data for this slot
    bxr_audio_t_ *audio = &audios_[audio_slot];
    if (!audio || !audio->data || audio->length == 0) {
      BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
      continue;
    }

    SDL_SetAudioStreamGain(voice->stream, voice->volume);

    // Calculate how much data is queued in the stream
    size_t bytes_per_sample = SDL_AUDIO_BYTESIZE(audio->spec.format);
    size_t bytes_per_frame  = bytes_per_sample * audio->spec.channels;
    size_t bytes_per_second = bytes_per_frame * audio->spec.freq;

    double target_latency_seconds = BXR_AUDIO_TARGET_LATENCY_MS / 1000.0;

    // SDL accept whole frame sizes, so we need to round down to the nearest
    // frame size
    size_t target_queue_bytes
        = (size_t)(bytes_per_second * target_latency_seconds);
    int remainder = target_queue_bytes % bytes_per_frame;
    target_queue_bytes -= remainder;

    if (SDL_GetAudioStreamQueued(voice->stream) < target_queue_bytes) {
      // Guard: If the voice position is already at or beyond the audio length,
      // handle looping or stopping
      if (voice->position >= audio->length) {
        if (voice->one_shot) {
          voice->feeding  = false;
          voice->position = 0;
          continue;
        } else {
          // Loop back to the beginning
          voice->position = 0;
        }
      }

      // Calculate how much data to queue
      Uint64 remaining = audio->length - (Uint64)voice->position;
      int chunk_size
          = remaining < target_queue_bytes ? remaining : target_queue_bytes;

      if (chunk_size == 0) {
        voice->position = 0;
        continue;
      }

      // Queue the audio data to the stream
      if (SDL_PutAudioStreamData(
              voice->stream, audio->data + voice->position, chunk_size)
          == false) {
        BXR_LOG_ERROR(
            "Failed to queue audio data for voice %d: %s", i, SDL_GetError());
        continue;
      }

      // Update the voice position
      voice->position += chunk_size;

      // If we've reached the end of the audio data, stop the voice
      if (voice->position >= audio->length) {
        if (voice->one_shot) {
          voice->feeding  = false;
          voice->position = 0;
        } else {
          // Loop back to the beginning
          voice->position = 0;
        }
      }
    }
  }
}

void
bxr_audio_teardown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  initialized_ = 0;

  bxr_audio_release_();

  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/**
 * ## Public API
 */

/* Load WAV via SDL_LoadWAV */
static bxr_audio_t_ *
bxr_audio_load_wav_(const char *path)
{
  bxr_audio_t_ *audio = NULL;
  BXR_NEW(audio);
  if (!audio) {
    return NULL;
  }

  SDL_zero(audio->spec);
  audio->data   = NULL;
  audio->length = 0;

  if (SDL_LoadWAV_IO(PHYSFSSDL3_openRead(path),
                     true,
                     &audio->spec,
                     &audio->data,
                     &audio->length)
      == false) {
    BXR_LOG_ERROR(
        "Failed to load WAV file: %s (error: %s)", path, SDL_GetError());
    BXR_FREE(audio);
    return NULL;
  }

  return audio;
}

/* Load OGG via stb_vorbis */
static bxr_audio_t_ *
bxr_audio_load_ogg_(const char *path)
{
  bxr_audio_t_ *audio = NULL;
  BXR_NEW(audio);
  if (!audio) {
    return NULL;
  }

  int channels    = 0;
  int sample_rate = 0;
  short *decoded  = NULL;

  size_t file_size = 0;
  Uint8 *file_data = bxr_asset_read(path, &file_size);
  if (!file_data || file_size == 0) {
    BXR_LOG_ERROR("Failed to read OGG file: %s", path);
    BXR_FREE(audio);
    return NULL;
  }

  int samples = stb_vorbis_decode_memory(
      file_data, file_size, &channels, &sample_rate, &decoded);
  if (samples < 0 || decoded == NULL) {
    BXR_LOG_ERROR("Failed to load OGG file: %s", path);
    BXR_FREE(audio);
    BXR_FREE(file_data);
    return NULL;
  }

  BXR_FREE(file_data);

  audio->data   = (Uint8 *)decoded;
  audio->length = (Uint32)(samples * channels * sizeof(short));

  audio->spec.freq     = sample_rate;
  audio->spec.format   = SDL_AUDIO_S16;
  audio->spec.channels = (Uint8)channels;

  return audio;
}

bxr_audio_t
bxr_audio_create(const char *path)
{
  BXR_ASSERT(path);
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  BXR_LOG_INFO("Loading audio from: %s", path);

  if (!bxr_asset_exists(path)) {
    BXR_LOG_ERROR("File not found: %s", path);
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  bxr_audio_t_ *audio = NULL;

  if (bxr_asset_has_extension(path, ".wav")) {
    audio = bxr_audio_load_wav_(path);
  } else if (bxr_asset_has_extension(path, ".ogg")) {
    audio = bxr_audio_load_ogg_(path);
  } else {
    BXR_LOG_ERROR("Unsupported audio format: %s", path);
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  if (!audio) {
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  int slot = bxr_pool_aquire_slot(audio_pool_);
  if (slot == BXR_POOL_INVALID_SLOT) {
    // no free slots
    if (audio->data) {
      BXR_FREE(audio->data);
    }
    BXR_FREE(audio);
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  audios_[slot] = *audio;
  // we copied contents, free temporary structure pointer
  BXR_FREE(audio);

  return (bxr_audio_t){ .id = bxr_pool_generate_id(audio_pool_, slot) };
}

Uint64
bxr_audio_get_length(bxr_audio_t audio)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid audio ID");
    return 0;
  }

  int slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Audio slot not found for ID: %u", audio.id);
    return 0;
  }

  bxr_audio_t_ *a = &audios_[slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", slot);
    return 0;
  }

  Uint64 byte_per_sec
      = a->spec.freq * a->spec.channels * SDL_AUDIO_BYTESIZE(a->spec.format);
  if (byte_per_sec == 0) {
    return 0;
  }

  return ((Uint64)a->length * 1000) / byte_per_sec; // length in milliseconds
}

void
bxr_audio_destroy(bxr_audio_t audio)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid audio ID");
    return;
  }

  int slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Audio slot not found for ID: %u", audio.id);
    return;
  }

  // Release any voices that are using this audio
  for (int i = 0; i < BXR_AUDIO_MAX_TRACKS; ++i) {
    bxr_voice_t_ *t = &voices_[i];

    if (t->stream && t->audio_slot >= 0) {
      int audio_slot = t->audio_slot;
      if (bxr_pool_get_slot_from_id(audio_pool_, audio.id) == audio_slot) {
        bxr_voice_release_(i);
      }
    }
  }

  BXR_FREE(audios_[slot].data);
  audios_[slot] = (bxr_audio_t_){ 0 };

  bxr_pool_release_slot(audio_pool_, slot);
}

void
bxr_audio_set_global_volume(float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  volume = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);

  if (!SDL_SetAudioDeviceGain(device_id_, volume)) {
    BXR_LOG_ERROR("Failed to set global volume: %s", SDL_GetError());
  }
}

float
bxr_audio_get_global_volume(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return 0.0f;
  }

  return SDL_GetAudioDeviceGain(device_id_);
}

static bxr_voice_t
bxr_audio_play_(bxr_audio_t audio, float volume, bool auto_play, bool one_shot)
{
  // Check if the audio device is initialized
  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio ID is valid
  if (audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid audio ID");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the audio slot from the pool
  int audio_slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (audio_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Audio slot not found for ID: %u", audio.id);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio data is valid
  bxr_audio_t_ *a = &audios_[audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Borrow a voice slot for playback
  int slot = bxr_pool_aquire_slot(voice_pool_);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("No free voice slots available");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the voice and set it up
  bxr_voice_t_ *t = &voices_[slot];
  t->audio_slot   = audio_slot;
  t->volume = volume = bxr_clamp(volume, 0.0f, 1.0f);
  t->position        = 0;
  t->feeding         = auto_play;
  t->one_shot        = one_shot;
  t->tag_count       = 0;

  t->stream = SDL_CreateAudioStream(&a->spec, &device_spec_);
  if (!t->stream) {
    BXR_LOG_ERROR("Failed to create audio stream for voice: %s",
                  SDL_GetError());
    bxr_pool_release_slot(voice_pool_, slot);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  if (SDL_BindAudioStream(device_id_, t->stream) == false) {
    BXR_LOG_ERROR("Failed to bind audio stream to device: %s", SDL_GetError());
    bxr_pool_release_slot(voice_pool_, slot);
    SDL_DestroyAudioStream(t->stream);
    t->stream = NULL;
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  SDL_SetAudioStreamGain(t->stream, volume);

  bxr_voice_t voice_id = { .id = bxr_pool_generate_id(voice_pool_, slot) };
  t->voice             = voice_id;

  return voice_id;
}

void
bxr_audio_play_once(bxr_audio_t audio, float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_audio_play_(audio, volume, true, true);
}

bxr_voice_t
bxr_voice_create(bxr_audio_t audio, float volume, bool auto_play)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return bxr_audio_play_(audio, volume, auto_play, false);
}

void
bxr_voice_set_volume(bxr_voice_t voice, float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  t->volume       = bxr_clamp(volume, 0.0f, 1.0f);
}

float
bxr_voice_get_volume(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return 0.0f;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return 0.0f;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return 0.0f;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  return t->volume;
}

void
bxr_voice_play(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  t->feeding      = true;
}

bool
bxr_voice_is_paused(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return false;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return false;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return false;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  return !t->feeding;
}

void
bxr_voice_pause(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  t->feeding      = false;
}

void
bxr_voice_stop(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  t->feeding      = false;
  t->position     = 0;
  SDL_ClearAudioStream(t->stream);
}

Uint64
bxr_voice_get_position(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return 0;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return 0;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  if (t->audio_slot < 0 || t->audio_slot >= BXR_AUDIO_MAX) {
    BXR_LOG_ERROR("Invalid audio slot for voice ID: %u", voice.id);
    return 0;
  }

  bxr_audio_t_ *a = &audios_[t->audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", t->audio_slot);
    return 0;
  }

  Uint64 byte_per_sec
      = a->spec.freq * a->spec.channels * SDL_AUDIO_BYTESIZE(a->spec.format);
  if (byte_per_sec == 0) {
    return 0;
  }

  Uint64 position
      = (t->position * 1000) / byte_per_sec; // position in milliseconds

  Uint64 queued_bytes = SDL_GetAudioStreamQueued(t->stream);
  Uint64 queued_ms    = (queued_bytes * 1000) / byte_per_sec;

  // Add queued time to the current position in order to get the total playback
  // position
  return position += queued_ms;
}

bool
bxr_voice_set_position(bxr_voice_t voice, Uint64 position_ms)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return false;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return false;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];
  if (t->audio_slot < 0 || t->audio_slot >= BXR_AUDIO_MAX) {
    BXR_LOG_ERROR("Invalid audio slot for voice ID: %u", voice.id);
    return false;
  }

  bxr_audio_t_ *a = &audios_[t->audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", t->audio_slot);
    return false;
  }

  Uint64 frame_size     = a->spec.channels * SDL_AUDIO_BYTESIZE(a->spec.format);
  Uint64 frames         = a->spec.freq * position_ms / 1000;
  Uint64 position_bytes = frames * frame_size;

  if (position_bytes >= a->length) {
    BXR_LOG_WARN("Position %llu ms exceeds audio length for voice ID: %u",
                 position_ms,
                 voice.id);
    return false;
  }

  t->position = position_bytes;
  // Clear the audio stream to reset the queued data and start from the new
  // position
  SDL_ClearAudioStream(t->stream);

  return true;
}

void
bxr_voice_destroy(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return;
  }

  bxr_voice_release_(voice_slot);
}

bool
bxr_audio_tag_add(const char *tag_name, bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return false;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return false;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];

  if (t->tag_count >= BXR_AUDIO_MAX_TAGS) {
    BXR_LOG_ERROR("Track has reached maximum number of tags");
    return false;
  }

  // Check if the tag already exists for this voice
  for (size_t i = 0; i < t->tag_count; ++i) {
    if (bxr_strcmp(t->tags[i], tag_name) == 0) {
      BXR_LOG_WARN(
          "Tag '%s' already exists for voice ID: %u", tag_name, voice.id);
      return true;
    }
  }

  // Add the tag to the voice's tag list
  t->tags[t->tag_count] = bxr_strdup(tag_name);
  if (!t->tags[t->tag_count]) {
    BXR_LOG_ERROR("Failed to allocate memory for tag name");
    return false;
  }
  t->tag_count++;

  return bxr_multimap_add(audio_tags_, tag_name, voice.id);
}

bool
bxr_audio_tag_remove(const char *tag_name, bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return false;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return false;
  }

  bxr_voice_t_ *t = &voices_[voice_slot];

  if (t->tag_count == 0) {
    return true; // No tags to remove
  }

  // Remove the tag from the voice's tag list
  for (size_t i = 0; i < t->tag_count; ++i) {
    if (bxr_strcmp(t->tags[i], tag_name) == 0) {
      BXR_FREE(t->tags[i]);
      // Shift remaining tags down
      for (size_t j = i; j < t->tag_count - 1; ++j) {
        t->tags[j] = t->tags[j + 1];
      }
      t->tags[t->tag_count - 1] = NULL;
      t->tag_count--;
      break;
    }
  }

  return bxr_multimap_remove_value(audio_tags_, tag_name, voice.id);
}

void
bxr_audio_tag_set_volume(const char *tag_name, float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  Uint32 *out_values = NULL;
  size_t values_count
      = bxr_multimap_get_values(audio_tags_, tag_name, &out_values);

  for (size_t i = 0; i < values_count; ++i) {
    bxr_voice_t voice = { .id = out_values[i] };
    bxr_voice_set_volume(voice, volume);
  }

  BXR_FREE(out_values);
}

void
bxr_audio_tag_play(const char *tag_name)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  Uint32 *out_values = NULL;
  size_t values_count
      = bxr_multimap_get_values(audio_tags_, tag_name, &out_values);

  for (size_t i = 0; i < values_count; ++i) {
    bxr_voice_t voice = { .id = out_values[i] };
    bxr_voice_play(voice);
  }

  BXR_FREE(out_values);
}

void
bxr_audio_tag_pause(const char *tag_name)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  Uint32 *out_values = NULL;
  size_t values_count
      = bxr_multimap_get_values(audio_tags_, tag_name, &out_values);

  for (size_t i = 0; i < values_count; ++i) {
    bxr_voice_t voice = { .id = out_values[i] };
    bxr_voice_pause(voice);
  }

  BXR_FREE(out_values);
}

void
bxr_audio_tag_stop(const char *tag_name)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  Uint32 *out_values = NULL;
  size_t values_count
      = bxr_multimap_get_values(audio_tags_, tag_name, &out_values);

  for (size_t i = 0; i < values_count; ++i) {
    bxr_voice_t voice = { .id = out_values[i] };
    bxr_voice_stop(voice);
  }

  BXR_FREE(out_values);
}
