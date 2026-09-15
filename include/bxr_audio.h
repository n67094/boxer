#ifndef BXR_AUDIO_H_
#define BXR_AUDIO_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

#define BXR_AUDIO_INVALID_ID 0

typedef struct bxr_audio_s
{
  Uint32 id;
} bxr_audio_t;

/**
 * ## Internal API
 */

/**
 * Initialize the image subsystem. Must be called before using any other image
 * functions.
 */
BXR_API void bxr_audio_setup(void);

/**
 * Update the audio subsystem. Must be called once per frame.
 */
BXR_API void bxr_audio_update(void);

/**
 * Tears down the image subsystem.
 */
BXR_API void bxr_audio_teardown(void);

/**
 * ## Public API
 */

BXR_API bxr_audio_t bxr_audio_create(const char *path);
BXR_API void bxr_audio_destroy(bxr_audio_t audio);

// Global controls
BXR_API void bxr_audio_set_global_volume(float volume);
BXR_API float bxr_audio_get_global_volume(void);

// Playback
BXR_API void bxr_audio_play(bxr_audio_t audio);
BXR_API void bxr_audio_stop(bxr_audio_t audio);
BXR_API void bxr_audio_pause(bxr_audio_t audio);
BXR_API void bxr_audio_resume(bxr_audio_t audio);
BXR_API bool bxr_audio_is_playing(bxr_audio_t audio);
BXR_API bool bxr_audio_is_paused(bxr_audio_t audio);

// Controls
BXR_API void bxr_audio_set_volume(bxr_audio_t audio, float volume);
BXR_API float bxr_audio_get_volume(bxr_audio_t audio);
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

#endif // BXR_AUDIO_H_
