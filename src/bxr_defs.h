/**
 * @file bxr_defs.h
 *
 * @brief Common definitions and macros.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_DEFS_H_
#define BXR_DEFS_H_

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BXR_INLINE static inline __attribute((always_inline))
#else
#define INLINE
#endif

#define BXR_INVALID_ID 0
#define BXR_IMPOSSIBLE_ID 0xFFFFFFFF

#define BXR_INIT_COOKIE                                                        \
  0xC0DED1ED // Magic value to indicate that a subsystem has been initialized

#define BXR_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

#endif // BXR_DEFS_H_
