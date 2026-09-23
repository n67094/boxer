/**
 * @file bxr_audio.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * TODO rename group to tag ?
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
BXR_API Uint32 bxr_audio_get_length(bxr_audio_t audio); // ms
BXR_API void bxr_audio_destroy(bxr_audio_t audio);

// Global controls
BXR_API void bxr_audio_set_global_volume(float volume);
BXR_API float bxr_audio_get_global_volume(void);
// TODO pan
// TODO pitch

BXR_API void bxr_audio_play_once(bxr_audio_t audio, float volume);

// Track playback
BXR_API bxr_track_t bxr_track_create(bxr_audio_t audio,
                                     float volume,
                                     bool auto_play);
BXR_API void bxr_track_set_volume(bxr_track_t track, float volume);
BXR_API float bxr_track_get_volume(bxr_track_t track);
BXR_API void bxr_track_play(bxr_track_t track);
BXR_API void bxr_track_pause(bxr_track_t track);
BXR_API bool bxr_track_is_paused(bxr_track_t track);
BXR_API void bxr_track_stop(bxr_track_t track);
BXR_API Uint32 bxr_track_get_position(bxr_track_t track); // ms
BXR_API bool bxr_track_set_position(bxr_track_t track, Uint32 position_ms);
// TODO pan
// TODO pitch
// TODO callback for when the track finishes playing
BXR_API void bxr_track_destroy(bxr_track_t track);

// Groups
BXR_API bool bxr_audio_group_add(const char *group_name, bxr_track_t track);
BXR_API bool bxr_audio_group_remove(const char *group_name, bxr_track_t track);

// Group playback
BXR_API void bxr_audio_group_set_volume(const char *group_name, float volume);
BXR_API void bxr_audio_group_play(const char *group_name);
BXR_API void bxr_audio_group_pause(const char *group_name);
BXR_API void bxr_audio_group_stop(const char *group_name);

#endif // BXR_AUDIO_H_
