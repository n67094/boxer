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
 * Tears down the image subsystem.
 */
BXR_API void bxr_audio_teardown(void);

/**
 * ## Public API
 */

BXR_API bxr_audio_t bxr_audio_create(const char *path);

BXR_API void bxr_audio_destroy(bxr_audio_t audio);

#endif // BXR_AUDIO_H_
