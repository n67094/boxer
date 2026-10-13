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
#include "bxr_string.h"

typedef enum
{
  BXR_AUDIO_FORMAT_UNKNOWN = SDL_AUDIO_UNKNOWN,
  BXR_AUDIO_U8             = SDL_AUDIO_U8,
  BXR_AUDIO_S8             = SDL_AUDIO_S8,
  BXR_AUDIO_S16LE          = SDL_AUDIO_S16LE,
  BXR_AUDIO_S16BE          = SDL_AUDIO_S16BE,
  BXR_AUDIO_S32LE          = SDL_AUDIO_S32LE,
  BXR_AUDIO_S32BE          = SDL_AUDIO_S32BE,
  BXR_AUDIO_F32LE          = SDL_AUDIO_F32LE,
  BXR_AUDIO_F32BE          = SDL_AUDIO_F32BE,
  BXR_AUDIO_S16            = SDL_AUDIO_S16,
  BXR_AUDIO_S32            = SDL_AUDIO_S32,
  BXR_AUDIO_F32            = SDL_AUDIO_F32
} bxr_audio_format_e;

typedef struct bxr_audio_s_
{
  Uint8 *data;
  Uint32 length; // in bytes
  SDL_AudioSpec spec;
} bxr_audio_t_;

typedef struct bxr_voice_s_
{
  bxr_voice_t voice;
  SDL_AudioStream *stream;
  int audio_slot;
  float volume;
  Uint32 position; // in bytes
  bool feeding;
  bool one_shot;
  char *tags[BXR_AUDIO_MAX_TAGS]; // tags associated with this voice
  size_t tag_count;
  float pitch; // pan (0.01 to 100.0, 1.0 is the base)
  float pan;   // pan adjustment (-1.0 left, 0.0 center, 1.0 right)
  int channels;
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
  bxr_voice_t_ *v = &voices_[voice_slot];
  for (size_t i = 0; i < v->tag_count; ++i) {
    bxr_multimap_remove_value(audio_tags_, v->tags[i], v->voice.id);
    BXR_FREE(v->tags[i]);
  }
  v->audio_slot = -1;
  v->volume     = 1.0f;
  v->position   = 0;
  v->feeding    = false;
  v->one_shot   = false;
  v->tag_count  = 0;
  if (v->stream) {
    SDL_DestroyAudioStream(v->stream);
    v->stream = NULL;
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
    for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
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

  audios_ = BXR_CALLOC(BXR_AUDIO_MAX, sizeof(bxr_audio_t_));
  if (!audios_) {
    BXR_LOG_ERROR("Failed to allocate audio slots");
    bxr_audio_release_();
    return;
  }

  // Create a pool for audio voices
  voice_pool_ = bxr_pool_create(BXR_AUDIO_MAX_VOICES);
  if (!voice_pool_) {
    BXR_LOG_ERROR("Failed to create voice pool");
    bxr_audio_release_();
    return;
  }

  voices_ = BXR_ALLOC(sizeof(bxr_voice_t_) * BXR_AUDIO_MAX_VOICES);
  if (!voices_) {
    BXR_LOG_ERROR("Failed to allocate voice slots");
    bxr_audio_release_();
    return;
  }

  for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
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

static bool
bxr_audio_put_panned_(bxr_voice_t_ *voice,
                      const float *in,
                      int frames,
                      int channels)
{
  float out[BXR_AUDIO_LOAD_FRAMES * 2];

  const float l = (voice->pan > 0.0f) ? 1.0f - voice->pan : 1.0f;
  const float r = (voice->pan < 0.0f) ? 1.0f + voice->pan : 1.0f;

  while (frames > 0) {
    int n = frames < BXR_AUDIO_LOAD_FRAMES ? frames : BXR_AUDIO_LOAD_FRAMES;

    if (channels == 1) {
      for (int i = 0; i < n; ++i) {
        out[i * 2 + 0] = in[i] * l;
        out[i * 2 + 1] = in[i] * r;
      }
    } else { /* stereo */
      for (int i = 0; i < n; ++i) {
        out[i * 2 + 0] = in[i * 2 + 0] * l;
        out[i * 2 + 1] = in[i * 2 + 1] * r;
      }
    }

    if (!SDL_PutAudioStreamData(voice->stream, out, n * 2 * (int)sizeof(float)))
      return false;

    in += n * channels;
    frames -= n;
  }
  return true;
}

void
bxr_audio_begin_frame(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
    bxr_voice_t_ *voice = &voices_[i];

    // Skip voices that are not active
    if (!voice->stream) {
      continue;
    }

    // Release finished one-shot voices to free up resources
    if (voice->one_shot && !voice->feeding
        && SDL_GetAudioStreamQueued(voice->stream) == 0) {
      bxr_voice_release_(i);
    }

    // Skip voices that are not feeding audio data
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
    if (!audio->data || audio->length == 0) {
      BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
      continue;
    }

    SDL_SetAudioStreamGain(voice->stream, voice->volume);

    // Panned voices are stereo, others keep the source channel count.
    size_t load_bytes = BXR_AUDIO_LOAD_FRAMES * voice->channels * sizeof(float);
    if (SDL_GetAudioStreamQueued(voice->stream) >= load_bytes) {
      // Already have enough queued data, skip feeding
      continue;
    }

    // Calculate the number of bytes for a single frame
    const Uint64 single_frame = (Uint64)audio->spec.channels * sizeof(float);

    // Guard: If the voice position is already at or beyond the audio length,
    // handle looping or stopping for one-shot voices.
    if (voice->position + single_frame >= audio->length) {
      voice->position = 0;
      if (voice->one_shot) {
        voice->feeding = false;
        continue; // Skip feeding for one-shot voices that have finished
      }
    }

    // Calculate the remaining frames to feed (in case the audio is shorter than
    // the chunk size)
    Uint64 remaining_frames
        = (audio->length - (Uint64)voice->position) / single_frame;

    // Determine how many frames to feed in this iteration (up to
    // BXR_AUDIO_LOAD_FRAMES)
    int frames = remaining_frames < (Uint64)BXR_AUDIO_LOAD_FRAMES
                     ? (int)remaining_frames
                     : BXR_AUDIO_LOAD_FRAMES;

    if (frames == 0) {
      continue;
    }

    // Pan and queue the audio data to the stream. Sources with more than two
    // channels are not panned and are queued as is.
    const float *src = (const float *)(audio->data + voice->position);
    bool queued
        = audio->spec.channels <= 2
              ? bxr_audio_put_panned_(voice, src, frames, audio->spec.channels)
              : SDL_PutAudioStreamData(
                    voice->stream, src, frames * (int)single_frame);
    if (!queued) {
      BXR_LOG_ERROR(
          "Failed to queue audio data for voice %d: %s", i, SDL_GetError());
      continue;
    }

    // Update the voice position
    voice->position += (Uint64)frames * single_frame;

    // If we've reached the end of the audio data, stop the voice
    if (voice->position + single_frame >= audio->length) {
      voice->position = 0;
      if (voice->one_shot) {
        voice->feeding = false;
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

  // Convert audio to float for pan support
  SDL_AudioSpec dst = { SDL_AUDIO_F32, audio->spec.channels, audio->spec.freq };
  Uint8 *converted  = NULL;
  int converted_len = 0;

  if (!SDL_ConvertAudioSamples(&audio->spec,
                               audio->data,
                               (int)audio->length,
                               &dst,
                               &converted,
                               &converted_len)) {
    BXR_LOG_ERROR("Failed to convert audio samples: %s", SDL_GetError());
    if (audio->data) {
      BXR_FREE(audio->data);
    }
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  BXR_FREE(audio->data);
  audio->data   = converted;
  audio->length = converted_len;
  audio->spec   = dst;

  int slot = bxr_pool_acquire_slot(audio_pool_);
  if (slot == BXR_POOL_INVALID_SLOT) {
    // No free slots
    if (audio->data) {
      BXR_FREE(audio->data);
    }
    BXR_FREE(audio);
    return (bxr_audio_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  audios_[slot] = *audio;
  // We copied contents, free temporary structure pointer
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
  for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
    bxr_voice_t_ *v = &voices_[i];
    if (v->stream && v->audio_slot >= 0) {
      int audio_slot = v->audio_slot;
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

static bxr_voice_t_ *
bxr_voice_get_(bxr_voice_t voice)
{
  if (voice.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid voice ID");
    return NULL;
  }

  int voice_slot = bxr_pool_get_slot_from_id(voice_pool_, voice.id);
  if (voice_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", voice.id);
    return NULL;
  }

  return &voices_[voice_slot];
}

bxr_voice_t
bxr_voice_create(const bxr_voice_desc_t *desc)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(desc);

  // Check if the audio device is initialized
  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio ID is valid
  if (desc->audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid audio ID");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the audio slot from the pool
  int audio_slot = bxr_pool_get_slot_from_id(audio_pool_, desc->audio.id);
  if (audio_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Audio slot not found for ID: %u", desc->audio.id);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio data is valid
  bxr_audio_t_ *a = &audios_[audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Borrow a voice slot for playback
  int slot = bxr_pool_acquire_slot(voice_pool_);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("No free voice slots available");
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the voice and set it up
  bxr_voice_t_ *v = &voices_[slot];
  v->audio_slot   = audio_slot;
  v->volume       = bxr_clamp(desc->volume, 0.0f, 1.0f);
  v->position     = 0;
  v->feeding      = desc->auto_play;
  v->one_shot     = desc->play_once;
  v->tag_count    = 0;
  v->pitch        = 1.0f;
  v->pan          = 0.0f;
  v->stream       = SDL_CreateAudioStream(&a->spec, &device_spec_);
  if (!v->stream) {
    BXR_LOG_ERROR("Failed to create audio stream for voice: %s",
                  SDL_GetError());
    bxr_pool_release_slot(voice_pool_, slot);
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Force mono and stereo streams to be stereo for panning support. Sources
  // with more channels keep their layout and let SDL map it to the device.
  v->channels = a->spec.channels;
  if (a->spec.channels <= 2) {
    SDL_AudioSpec in = { a->spec.format, 2, a->spec.freq };
    SDL_SetAudioStreamFormat(v->stream, &in, NULL);
    v->channels = 2;
  }

  if (SDL_BindAudioStream(device_id_, v->stream) == false) {
    BXR_LOG_ERROR("Failed to bind audio stream to device: %s", SDL_GetError());
    bxr_pool_release_slot(voice_pool_, slot);
    SDL_DestroyAudioStream(v->stream);
    v->stream = NULL;
    return (bxr_voice_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  SDL_SetAudioStreamGain(v->stream, desc->volume);

  bxr_voice_t voice_id = { .id = bxr_pool_generate_id(voice_pool_, slot) };
  v->voice             = voice_id;

  return voice_id;
}

void
bxr_voice_set_volume(bxr_voice_t voice, float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return;
  }

  v->volume = bxr_clamp(volume, 0.0f, 1.0f);
}

float
bxr_voice_get_volume(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return 0.0f;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return 0.0f;
  }
  return v->volume;
}

void
bxr_voice_play(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return;
  }
  v->feeding = true;
}

bool
bxr_voice_is_paused(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return false;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }
  return !v->feeding;
}

void
bxr_voice_pause(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return;
  }
  v->feeding = false;
}

void
bxr_voice_stop(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return;
  }
  v->feeding  = false;
  v->position = 0;
  SDL_ClearAudioStream(v->stream);
}

Uint64
bxr_voice_get_position(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return 0;
  }

  if (v->audio_slot < 0 || v->audio_slot >= BXR_AUDIO_MAX) {
    BXR_LOG_ERROR("Invalid audio slot for voice ID: %u", voice.id);
    return 0;
  }

  bxr_audio_t_ *a = &audios_[v->audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", v->audio_slot);
    return 0;
  }

  // The audio data keeps the source channel count, but the stream input may be
  // forced to stereo for pan support, so each side has its own frame size.
  const Uint64 bytesize     = SDL_AUDIO_BYTESIZE(a->spec.format);
  const Uint64 src_frame    = (Uint64)a->spec.channels * bytesize;
  const Uint64 stream_frame = (Uint64)v->channels * bytesize;
  if (src_frame == 0 || a->spec.freq <= 0) {
    return 0;
  }

  Uint64 position_frames = v->position / src_frame;

  int queued_bytes = SDL_GetAudioStreamQueued(v->stream);
  if (queued_bytes < 0) {
    queued_bytes = 0;
  }
  Uint64 queued_frames = (Uint64)queued_bytes / stream_frame;

  // The position wraps to 0 when the end of the audio is fed, while its tail
  // may still be queued.
  if (queued_frames > position_frames) {
    if (v->one_shot) {
      return 0;
    }
    position_frames += a->length / src_frame;
    if (queued_frames > position_frames) {
      return 0;
    }
  }

  // Subtract the queued frames from the position to get the actual playback
  // position, then convert to milliseconds
  return (position_frames - queued_frames) * 1000 / (Uint64)a->spec.freq;
}

bool
bxr_voice_set_position(bxr_voice_t voice, Uint64 position_ms)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }

  if (v->audio_slot < 0 || v->audio_slot >= BXR_AUDIO_MAX) {
    BXR_LOG_ERROR("Invalid audio slot for voice ID: %u", voice.id);
    return false;
  }

  bxr_audio_t_ *a = &audios_[v->audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", v->audio_slot);
    return false;
  }

  Uint64 frame_size
      = (Uint64)a->spec.channels * SDL_AUDIO_BYTESIZE(a->spec.format);
  Uint64 frames         = a->spec.freq * position_ms / 1000;
  Uint64 position_bytes = frames * frame_size;

  if (position_bytes >= a->length) {
    BXR_LOG_WARN("Position %llu ms exceeds audio length for voice ID: %u",
                 position_ms,
                 voice.id);
    return false;
  }

  v->position = position_bytes;
  // Clear the audio stream to reset the queued data and start from the new
  // position
  SDL_ClearAudioStream(v->stream);

  return true;
}

bool
bxr_voice_set_pitch(bxr_voice_t voice, float pitch)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }

  pitch = bxr_clamp(pitch, 0.01f, 100.0f); // Clamp pitch to a reasonable range

  v->pitch = pitch;

  if (!SDL_SetAudioStreamFrequencyRatio(v->stream, pitch)) {
    SDL_Log("Failed to set pitch: %s", SDL_GetError());
    return false;
  }

  return true;
}

float
bxr_voice_get_pitch(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return 1.0f; // Return default pitch if voice is invalid
  }

  return v->pitch;
}

bool
bxr_voice_set_pan(bxr_voice_t voice, float pan)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }

  if (v->channels != 2) {
    BXR_LOG_WARN("Pan is only supported for mono and stereo voices, ID: %u",
                 voice.id);
    return false;
  }

  pan = bxr_clamp(pan, -1.0f, 1.0f);

  v->pan = pan;

  return true;
}

float
bxr_voice_get_pan(bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return 0.0f; // Return default pan if voice is invalid
  }

  return v->pan;
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

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }

  if (v->tag_count >= BXR_AUDIO_MAX_TAGS) {
    BXR_LOG_ERROR("Track has reached maximum number of tags");
    return false;
  }

  // Check if the tag already exists for this voice
  for (size_t i = 0; i < v->tag_count; ++i) {
    if (bxr_strcmp(v->tags[i], tag_name) == 0) {
      BXR_LOG_WARN(
          "Tag '%s' already exists for voice ID: %u", tag_name, voice.id);
      return true;
    }
  }

  // Add the tag to the voice's tag list
  v->tags[v->tag_count] = bxr_strdup(tag_name);
  if (!v->tags[v->tag_count]) {
    BXR_LOG_ERROR("Failed to allocate memory for tag name");
    return false;
  }
  v->tag_count++;

  return bxr_multimap_add(audio_tags_, tag_name, voice.id);
}

bool
bxr_audio_tag_remove(const char *tag_name, bxr_voice_t voice)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(tag_name);

  bxr_voice_t_ *v = bxr_voice_get_(voice);
  if (!v) {
    BXR_LOG_ERROR("Invalid voice ID: %u", voice.id);
    return false;
  }

  if (v->tag_count == 0) {
    return true; // No tags to remove
  }

  // Remove the tag from the voice's tag list
  for (size_t i = 0; i < v->tag_count; ++i) {
    if (bxr_strcmp(v->tags[i], tag_name) == 0) {
      BXR_FREE(v->tags[i]);
      // Shift remaining tags down
      for (size_t j = i; j < v->tag_count - 1; ++j) {
        v->tags[j] = v->tags[j + 1];
      }
      v->tags[v->tag_count - 1] = NULL;
      v->tag_count--;
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
