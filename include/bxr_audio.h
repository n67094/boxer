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

typedef struct bxr_audio_s
{
  Uint32 id;
} bxr_audio_t;

#define BXR_TRACK_INVALID_ID 0

typedef struct bxr_track_s
{
  Uint32 id;
} bxr_track_t;

/**
 * ## Internal API
 */

BXR_API void bxr_audio_setup(void);
BXR_API void bxr_audio_begin_frame(void);
BXR_API void bxr_audio_teardown(void);

/**
 * ## Public API
 */

BXR_API bxr_audio_t bxr_audio_create(const char *path);
BXR_API void bxr_audio_destroy(bxr_audio_t audio);

// Global controls
BXR_API void bxr_audio_set_global_volume(float volume);
BXR_API float bxr_audio_get_global_volume(void);

// Mixer controls
BXR_API bxr_track_t bxr_track_play(bxr_audio_t audio, bool auto_play);
BXR_API void bxr_track_play_once(bxr_audio_t audio, float volume);
BXR_API void bxr_track_set_volume(bxr_track_t track, float volume);
BXR_API float bxr_track_get_volume(bxr_track_t track);
BXR_API bool bxr_track_is_paused(bxr_track_t track);
BXR_API void bxr_track_pause(bxr_track_t track);
BXR_API void bxr_track_resume(bxr_track_t track);
BXR_API void bxr_track_stop(bxr_track_t track);

/*
// Controls
BXR_API void bxr_audio_set_pan(bxr_audio_t audio,
                               float pan); // -1.0 left to 1.0 right
BXR_API void bxr_audio_set_loop(bxr_audio_t audio, bool loop);
BXR_API void bxr_audio_fade_in(bxr_audio_t audio, Uint32 duration_ms);
BXR_API void bxr_audio_fade_out(bxr_audio_t audio, Uint32 duration_ms);

// Seeking
BXR_API Uint32 bxr_audio_get_length(bxr_audio_t audio);   // ms
BXR_API Uint32 bxr_audio_get_position(bxr_audio_t audio); // ms
BXR_API void bxr_audio_set_position(bxr_audio_t audio, Uint32 position);

// Groups
BXR_API bool bxr_audio_group_add(bxr_audio_t audio, const char *group_name);
BXR_API bool bxr_audio_group_remove(bxr_audio_t audio, const char *group_name);

// Group playback
BXR_API void bxr_audio_group_play(const char *group_name);
BXR_API void bxr_audio_group_stop(const char *group_name);
BXR_API void bxr_audio_group_pause(const char *group_name);
BXR_API void bxr_audio_group_resume(const char *group_name);
BXR_API bool bxr_audio_group_is_playing(const char *group_name);
BXR_API bool bxr_audio_group_is_paused(const char *group_name);

// Group controls
BXR_API void bxr_audio_group_set_volume(const char *group_name, float volume);
BXR_API float bxr_audio_group_get_volume(const char *group_name);
BXR_API void bxr_audio_group_set_pan(const char *group_name, float pan);
BXR_API void bxr_audio_group_set_loop(const char *group_name, bool loop);
BXR_API void bxr_audio_group_fade_in(const char *group_name, float duration);
BXR_API void bxr_audio_group_fade_out(const char *group_name, float duration);
*/

#endif // BXR_AUDIO_H_
