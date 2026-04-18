/**
 * file `bxr_defs.h`
 *
 * copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Common definitions and macros.
 */

#ifndef BXR_DEFS_H_
#define BXR_DEFS_H_

#include <SDL_gp.h>

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BXR_INLINE static inline __attribute((always_inline))
#else
#define INLINE
#endif

// Magic value to indicate that a subsystem has been initialized
#define BXR_INIT_COOKIE 0xC0DED1ED

/**
 * Get the offset of a struct member in bytes.
 *
 * `TYPE` is the type of the struct.
 *
 * `ELEMENT` is the name of the struct member.
 *
 * `return` the offset of the struct member in bytes.
 */
#define BXR_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

#endif // BXR_DEFS_H_
