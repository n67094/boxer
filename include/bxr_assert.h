/**
 * File `bxr_assert.h`
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Assertion
 *
 * Asserts that an expression is true, and aborts the program if it is false.
 */

#ifndef BXR_ASSERT_H_
#define BXR_ASSERT_H_

#include <SDL3/SDL.h>

/**
 * ## Macros
 */

/**
 * Check that `expr` is true, and abort the program if it is false.
 */
#define BXR_ASSERT(expr) SDL_assert(expr)

#endif // BXR_ASSERT_H_
