/**
 * File `bxr_error.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Error Handling
 */

#ifndef BXR_ERROR_H_
#define BXR_ERROR_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

/**
 * ## API
 */

/**
 * Set the error message.
 *
 * Messages longer than `BXR_ERROR_MAX_LENGTH - 1` bytes are truncated.
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
 * Get the last error message.
 *
 * `return` the message, or an empty string if no error was set. Never NULL.
 *
 * The string is owned by boxer and stays valid until the next
 * `bxr_set_error()`.
 */
BXR_API const char *bxr_get_error(void);

/**
 * Clear the error message.
 */
BXR_API void bxr_clear_error(void);

#endif // BXR_ERROR_H_
