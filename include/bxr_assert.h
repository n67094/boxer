/**
 * File `bxr_assert.h`.
 *
 * # Assertion Macro.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ASSERT_H_
#define BXR_ASSERT_H_

#include <SDL3/SDL.h>

/**
 * Check that `expr` is true, and abort the program if it is false.
 */
#define BXR_ASSERT(expr) SDL_assert(expr)

#endif // BXR_ASSERT_H_
