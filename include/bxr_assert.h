/**
 * File `bxr_assert.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Assertion macro
 */

#ifndef BXR_ASSERT_H_
#define BXR_ASSERT_H_

/**
 * Assertion macro
 *
 * `expr` is the expression to evaluate. If the expression evaluates to false,
 * the program will be aborted.
 */
#define BXR_ASSERT(expr) SDL_assert(expr)

#endif // BXR_ASSERT_H_
