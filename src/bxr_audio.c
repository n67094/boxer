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
#include "bxr_pool.h"

typedef struct bxr_audio_s_
{
  Uint8 *data;
  Uint32 length;
  SDL_AudioSpec spec;
} bxr_audio_t_;

typedef struct bxr_track_s_
{
  SDL_AudioStream *stream;
  int audio_slot; // slot in audios_
  float volume;
  Uint32 position;
  bool playing;
  bool one_shot;
} bxr_track_t_;

static Uint32 initialized_ = 0;

static bxr_pool_t *audio_pool_ = NULL;
static bxr_audio_t_ *audios_   = NULL;

static bxr_pool_t *track_pool_ = NULL;
static bxr_track_t_ *tracks_   = NULL;

static SDL_AudioDeviceID device_id_ = 0;
static SDL_AudioSpec device_spec_;

/**
 * ## Internal API
 */

void
bxr_audio_setup(void)
{
  BXR_ASSERT(initialized_ == 0);
  initialized_ = BXR_INIT_COOKIE;

  // Open the default audio device for playback
  device_id_ = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (device_id_ == 0) {
    BXR_LOG_ERROR("Failed to open audio device: %s", SDL_GetError());
    return;
  }

  // Get the audio device format
  if (SDL_GetAudioDeviceFormat(device_id_, &device_spec_, NULL) != 0) {
    BXR_LOG_ERROR("Failed to get audio device format: %s", SDL_GetError());
    return;
  }

  // Audio pool for loaded audio slots
  audio_pool_ = bxr_pool_create(BXR_AUDIO_MAX);
  BXR_ALLOC(audios_, sizeof(bxr_audio_t_) * (BXR_AUDIO_MAX));
  memset(audios_, 0, sizeof(bxr_audio_t_) * (BXR_AUDIO_MAX));

  // Create a pool for audio tracks
  track_pool_ = bxr_pool_create(BXR_AUDIO_MAX_VOICES);
  BXR_ALLOC(tracks_, sizeof(bxr_track_t_) * BXR_AUDIO_MAX_VOICES);
  memset(tracks_, 0, sizeof(bxr_track_t_) * BXR_AUDIO_MAX_VOICES);

  for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
    tracks_[i].stream     = NULL;
    tracks_[i].audio_slot = -1;
    tracks_[i].volume     = 1.0f;
    tracks_[i].position   = 0;
    tracks_[i].playing    = false;
    tracks_[i].one_shot   = false;
  }

  SDL_ResumeAudioDevice(device_id_);
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
    bxr_track_t_ *track = &tracks_[i];

    // Skip tracks that are not playing or have no stream
    if (!track->playing || !track->stream) {
      continue;
    }

    const int target_queue = 8192; // 8192 bytes of audio data in the queue
    if (SDL_GetAudioStreamQueued(track->stream) < target_queue) {
      int audio_slot = track->audio_slot;
      if (audio_slot < 0 || audio_slot >= BXR_AUDIO_MAX) {
        BXR_LOG_ERROR("Invalid audio slot for track %d", i);
        continue;
      }

      bxr_audio_t_ *audio = &audios_[audio_slot];
      if (!audio || !audio->data || audio->length == 0) {
        BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
        continue;
      }

      // Calculate how much data to queue
      Uint32 remaining  = audio->length - track->position;
      Uint32 chunk_size = remaining < target_queue ? remaining : target_queue;

      // Queue the audio data to the stream
      if (SDL_PutAudioStreamData(
              track->stream, audio->data + track->position, chunk_size)
          != 0) {
        BXR_LOG_ERROR(
            "Failed to queue audio data for track %d: %s", i, SDL_GetError());
        continue;
      }

      // Update the track position
      track->position += chunk_size;

      // If we've reached the end of the audio data, stop the track
      if (track->position >= audio->length) {
        if (track->one_shot) {
          track->playing = false;
          SDL_ClearAudioStream(track->stream);
          track->position = 0;
        } else {
          // Loop back to the beginning
          track->position = 0;
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

  if (device_id_ != 0) {
    SDL_CloseAudioDevice(device_id_);
    device_id_ = 0;
  }

  if (tracks_) {
    for (int i = 0; i < BXR_AUDIO_MAX_VOICES; ++i) {
      if (tracks_[i].stream) {
        SDL_DestroyAudioStream(tracks_[i].stream);
        tracks_[i].stream = NULL;
      }
    }
    BXR_FREE(tracks_);
  }

  if (track_pool_) {
    bxr_pool_destroy(track_pool_);
    track_pool_ = NULL;
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

  if (SDL_LoadWAV(path, &audio->spec, &audio->data, &audio->length) == false) {
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

  int samples
      = stb_vorbis_decode_filename(path, &channels, &sample_rate, &decoded);
  if (samples < 0 || decoded == NULL) {
    BXR_LOG_ERROR("Failed to load OGG file: %s", path);
    BXR_FREE(audio);
    return NULL;
  }

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

void
bxr_audio_destroy(bxr_audio_t audio)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_WARN("Invalid audio ID");
    return;
  }

  int slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_WARN("Audio slot not found for ID: %u", audio.id);
    return;
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

  if (volume < 0.0f) {
    volume = 0.0f;
  } else if (volume > 1.0f) {
    volume = 1.0f;
  }

  SDL_SetAudioDeviceGain(device_id_, volume);
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

static bxr_track_t
bxr_audio_play_(bxr_audio_t audio, float volume, bool auto_play, bool one_shot)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  // Check if the audio device is initialized
  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return (bxr_track_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio ID is valid
  if (audio.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid audio ID");
    return (bxr_track_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the audio slot from the pool
  int audio_slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (audio_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Audio slot not found for ID: %u", audio.id);
    return (bxr_track_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Check if the audio data is valid
  bxr_audio_t_ *a = &audios_[audio_slot];
  if (!a || !a->data || a->length == 0) {
    BXR_LOG_ERROR("Audio data not available for slot %d", audio_slot);
    return (bxr_track_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Borrow a voice slot for playback
  int slot = bxr_pool_aquire_slot(track_pool_);
  if (slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("No free track slots available");
    return (bxr_track_t){ .id = BXR_AUDIO_INVALID_ID };
  }

  // Get the voice and set it up
  bxr_track_t_ *t = &tracks_[slot];
  t->audio_slot   = audio_slot;
  t->volume       = volume < 0.0f ? 1.0f : volume;
  t->position     = 0;
  t->playing      = auto_play;
  t->one_shot     = one_shot;
  t->stream       = SDL_CreateAudioStream(&a->spec, &device_spec_);

  SDL_ClearAudioStream(t->stream);
}

bxr_track_t
bxr_track_play(bxr_audio_t audio, bool auto_play)
{
  return bxr_audio_play_(audio, 1.0f, auto_play, false);
}

void
bxr_track_play_once(bxr_audio_t audio, float volume)
{
  bxr_audio_play_(audio, volume, true, true);
}

void
bxr_track_set_volume(bxr_track_t track, float volume)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  t->volume       = volume < 0.0f ? 1.0f : volume;
}

float
bxr_track_get_volume(bxr_track_t track)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return 0.0f;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return 0.0f;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return 0.0f;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  return t->volume;
}

bool
bxr_track_is_paused(bxr_track_t track)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return false;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return false;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return false;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  return !t->playing;
}

void
bxr_track_pause(bxr_track_t track)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  t->playing      = false;
}

void
bxr_track_resume(bxr_track_t track)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  t->playing      = true;
}

void
bxr_track_stop(bxr_track_t track)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (device_id_ == 0) {
    BXR_LOG_ERROR("Audio device not initialized");
    return;
  }

  if (track.id == BXR_AUDIO_INVALID_ID) {
    BXR_LOG_ERROR("Invalid track ID");
    return;
  }

  int track_slot = bxr_pool_get_slot_from_id(track_pool_, track.id);
  if (track_slot == BXR_POOL_INVALID_SLOT) {
    BXR_LOG_ERROR("Track slot not found for ID: %u", track.id);
    return;
  }

  bxr_track_t_ *t = &tracks_[track_slot];
  t->audio_slot   = -1;
  t->volume       = 1.0f;
  t->position     = 0;
  t->playing      = false;
  t->one_shot     = false;
  if (t->stream) {
    SDL_DestroyAudioStream(t->stream);
    t->stream = NULL;
  }

  bxr_pool_release_slot(track_pool_, track_slot);
}
