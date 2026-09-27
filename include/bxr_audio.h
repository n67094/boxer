/**
 * @file bxr_audio.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_AUDIO_H_
#define BXR_AUDIO_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

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

#define BXR_AUDIO_INVALID_ID 0

#define BXR_VOICE_INVALID_ID 0

typedef struct bxr_audio_s
{
  Uint32 id;
} bxr_audio_t;

typedef struct bxr_voice_s
{
  Uint32 id;
} bxr_voice_t;

typedef struct bxr_voice_desc_s
{
  bxr_audio_t audio;
  float volume;
  bool auto_play;
  bool play_once;
} bxr_voice_desc_t;

/**
 * ## Internal API
 */

void bxr_audio_setup(void);
void bxr_audio_begin_frame(void);
void bxr_audio_teardown(void);

/**
 * ## Public API
 */

BXR_API bxr_audio_t bxr_audio_create(const char *path);
BXR_API Uint64 bxr_audio_get_length(bxr_audio_t audio); // ms
BXR_API void bxr_audio_destroy(bxr_audio_t audio);

// Global controls
BXR_API void bxr_audio_set_global_volume(float volume);
BXR_API float bxr_audio_get_global_volume(void);
// TODO pan
// TODO pitch

// Track playback
BXR_API bxr_voice_t bxr_voice_create(bxr_voice_desc_t *desc);
BXR_API void bxr_voice_set_volume(bxr_voice_t voice, float volume);
BXR_API float bxr_voice_get_volume(bxr_voice_t voice);
BXR_API void bxr_voice_play(bxr_voice_t voice);
BXR_API void bxr_voice_pause(bxr_voice_t voice);
BXR_API bool bxr_voice_is_paused(bxr_voice_t voice);
BXR_API void bxr_voice_stop(bxr_voice_t voice);
BXR_API Uint64 bxr_voice_get_position(bxr_voice_t voice); // ms
BXR_API bool bxr_voice_set_position(bxr_voice_t voice, Uint64 position_ms);
// TODO pan
// TODO pitch
BXR_API void bxr_voice_destroy(bxr_voice_t voice);

// Tags
BXR_API bool bxr_audio_tag_add(const char *tag_name, bxr_voice_t voice);
BXR_API bool bxr_audio_tag_remove(const char *tag_name, bxr_voice_t voice);

// Group playback
BXR_API void bxr_audio_tag_set_volume(const char *tag_name, float volume);
BXR_API void bxr_audio_tag_play(const char *tag_name);
BXR_API void bxr_audio_tag_pause(const char *tag_name);
BXR_API void bxr_audio_tag_stop(const char *tag_name);

#endif // BXR_AUDIO_H_
