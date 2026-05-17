/**
 * File `bxr_assert.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Assertion macro
 */

#ifndef BXR_ASSERT_H_
#define BXR_ASSERT_H_

#define BXR_ASSERT(expr) ((void)((expr) || (SDL_assert(expr), 0)))

#endif // BXR_ASSERT_H_
