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
  Uint32 position;
  bool playing;
  float volume;

  SDL_AudioSpec spec;
} bxr_audio_t_;

static Uint32 initialized_       = 0;
static bxr_pool_t *audio_pool_   = NULL;
static bxr_audio_t_ *audios_     = NULL;
static bxr_pool_t *stream_pool_  = NULL;
static SDL_AudioStream *streams_ = NULL;

/**
 * ## Internal API
 */

BXR_API void
bxr_audio_setup(void)
{
  BXR_ASSERT(initialized_ == 0);
  initialized_ = BXR_INIT_COOKIE;

  audio_pool_ = bxr_pool_create(BXR_AUDIO_MAX);

  BXR_ALLOC(audios_, sizeof(bxr_audio_t_) * (BXR_AUDIO_MAX));
}

BXR_API void
bxr_audio_update(void)
{
  //
}

BXR_API void
bxr_audio_teardown(void)
{
  BXR_ASSERT(initialized_ == 0);
  initialized_ = BXR_INIT_COOKIE;

  if (audios_) {
    for (size_t i = 0; i < BXR_AUDIO_MAX; ++i) {
      if (audios_[i].data) {
        BXR_FREE(audios_[i].data);
      }
    }
    BXR_FREE(audios_);
    audios_ = NULL;
  }

  if (audio_pool_) {
    bxr_pool_destroy(audio_pool_);
    audio_pool_ = NULL;
  }
}

/**
 * ## Public API
 */

static bxr_audio_t_ *
bxr_audio_load_wav_(const char *path)
{
  bxr_audio_t_ *audio = NULL;
  BXR_NEW(audio);
  if (!audio) {
    return NULL;
  }

  SDL_LoadWAV(path, &audio->spec, &audio->data, &audio->length);
  if (!audio->data) {
    BXR_LOG_ERROR(
        "Failed to load WAV file: %s (error: %s)", path, SDL_GetError());
    BXR_FREE(audio);

    return (bxr_audio_t_ *)NULL;
  }

  audio->position = 0;

  return audio;
}

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
    BXR_LOG_ERROR("Failed to load OGG file: %s");
    BXR_FREE(audio);

    return (bxr_audio_t_ *)NULL;
  }

  audio->data     = (Uint8 *)decoded;
  audio->length   = (Uint32)(samples * channels * sizeof(short));
  audio->position = 0;

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
    return (bxr_audio_t){ .id = SDL_GP_INVALID_ID };
  }

  audios_[slot] = *audio;

  return (bxr_audio_t){ .id = bxr_pool_generate_id(audio_pool_, slot) };
}

void
bxr_audio_destroy(bxr_audio_t audio)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (audio.id == BXR_AUDIO_INVALID_ID) {
    return;
  }

  int slot = bxr_pool_get_slot_from_id(audio_pool_, audio.id);
  if (slot == BXR_POOL_INVALID_SLOT) {
    return;
  }

  BXR_FREE(audios_[slot].data);
  audios_[slot] = (bxr_audio_t_){ 0 };

  bxr_pool_release_slot(audio_pool_, slot);
}
