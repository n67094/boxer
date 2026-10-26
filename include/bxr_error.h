/**
 * File `bxr_error.h`.
 *
 * # Boxer's Error Handling.
 *
 * Works like `SDL_GetError()`: when a boxer function fails it returns false,
 * NULL or an invalid id, and stores a human-readable message that can be read
 * with `bxr_get_error()`.
 *
 * The message is only meaningful right after a function reported a failure,
 * successful calls do not clear it. Each thread has its own message.
 *
 * Basic usage:
 *
 *   if (!bxr_asset_mount("data.zip")) {
 *     BXR_LOG_ERROR("Couldn't mount data: %s", bxr_get_error());
 *   }
 *
 * Every message set is also logged at `BXR_LOG_PRIORITY_DEBUG`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ERROR_H_
#define BXR_ERROR_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

/**
 * ## Public API
 */

/**
 * Set the error message of the calling thread.
 *
 * The previous message can safely be used as an argument, e.g.
 * `bxr_set_error("Failed to load level: %s", bxr_get_error())`. Messages longer
 * than `BXR_ERROR_MAX_LENGTH - 1` bytes are truncated.
 *
 * `fmt` printf-style format string, followed by its arguments; must not be
 * NULL.
 *
 * `return` always false, so it can be returned directly from a failing
 * function.
 */
BXR_API bool bxr_set_error(SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
    SDL_PRINTF_VARARG_FUNC(1);

/**
 * Get the last error message of the calling thread.
 *
 * `return` the message, or an empty string if no error was set. Never NULL.
 * The string is owned by boxer and stays valid until the next
 * `bxr_set_error()` or `bxr_clear_error()` call on the same thread.
 */
BXR_API const char *bxr_get_error(void);

/**
 * Clear the error message of the calling thread.
 */
BXR_API void bxr_clear_error(void);

#endif // BXR_ERROR_H_
