/**
 * @file engine_defs.h
 *
 * @brief Common definitions and macros.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_DEFS_H_
#define ENGINE_DEFS_H_

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define ENGINE_INLINE static inline __attribute((always_inline))
#else
#define INLINE
#endif

#define ENGINE_INVALID_ID 0
#define ENGINE_IMPOSSIBLE_ID 0xFFFFFFFF

#define ENGINE_INIT_COOKIE                                                     \
  0xC0DED1ED // Magic value to indicate that a subsystem has been initialized

#define ENGINE_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

#endif // ENGINE_DEFS_H_
