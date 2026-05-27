/**
 * File `bxr_defs.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Common definitions and macros
 */

#ifndef BXR_DEFS_H_
#define BXR_DEFS_H_

#if defined(_WIN32)
#ifdef BOXER_BUILD
#define BXR_API __declspec(dllexport)
#else
#define BXR_API __declspec(dllimport)
#endif
#else
#define BXR_API extern
#endif

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BXR_INLINE static inline __attribute((always_inline))
#else
#define INLINE
#endif

#define BXR_ORGANIZATION_NAME "boxer"

// Magic value to indicate that a subsystem has been initialized
#define BXR_INIT_COOKIE 0xC0DED1ED

/**
 * This is useful for providing default values for function parameters that are
 * passed as 0.
 *
 * `val` is the value to check.
 *
 * `def` is the default value to return if `val` is 0.
 *
 * `return` `def` if `val` is 0, otherwise return `val`.
 */
#define BXR_DEFAULT(val, def) (((val) == 0) ? (def) : (val))

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

/**
 * Get the size of an array in elements.
 *
 * `arr` is the array to get the size of.
 *
 * `return` the size of the array in elements.
 */
#define BXR_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BXR_TERM_ESC "\x1B"
#define BXR_TERM_RESET BXR_TERM_ESC "[0m"
#define BXR_TERM_BOLD BXR_TERM_ESC "[1m"
#define BXR_TERM_COLOR_CYAN BXR_TERM_ESC "[36m"
#define BXR_TERM_COLOR_GREEN BXR_TERM_ESC "[32m"
#define BXR_TERM_COLOR_YELLOW BXR_TERM_ESC "[33m"
#define BXR_TERM_COLOR_RED BXR_TERM_ESC "[31m"

#endif // BXR_DEFS_H_
