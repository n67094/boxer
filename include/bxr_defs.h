/**
 * File `bxr_defs.h`.
 *
 * Common macros shared by every boxer module.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_DEFS_H_
#define BXR_DEFS_H_

/**
 * Marks a public function for export from (or import into) the boxer library.
 *
 * On Windows it expands to `__declspec(dllexport)` when `BOXER_BUILD` is
 * defined (building boxer) and `__declspec(dllimport)` otherwise. Elsewhere it
 * expands to `extern`.
 */
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
#define BXR_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BXR_INLINE static inline __attribute__((always_inline))
#else
#define BXR_INLINE static inline
#endif

#define BXR_ORGANIZATION_NAME "boxer"

#define BXR_INIT_COOKIE 0xC0DED1ED

/**
 * Returns `def` if `val` is 0, otherwise returns `val`.
 */
#define BXR_DEFAULT(val, def) (((val) == 0) ? (def) : (val))

/**
 * Returns the offset of `ELEMENT` in `TYPE`, in bytes.
 */
#define BXR_OFFSET_OF(TYPE, ELEMENT) SDL_offsetof(TYPE, ELEMENT)

/**
 * Returns the number of elements in an array. Must not be a pointer.
 */
#define BXR_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * ANSI terminal escape sequences, as string literals to concatenate with
 * other literals. Always end colored output with `BXR_TERM_RESET`.
 */
#define BXR_TERM_ESC "\x1B"
#define BXR_TERM_RESET BXR_TERM_ESC "[0m"
#define BXR_TERM_BOLD BXR_TERM_ESC "[1m"
#define BXR_TERM_COLOR_CYAN BXR_TERM_ESC "[36m"
#define BXR_TERM_COLOR_GREEN BXR_TERM_ESC "[32m"
#define BXR_TERM_COLOR_YELLOW BXR_TERM_ESC "[33m"
#define BXR_TERM_COLOR_RED BXR_TERM_ESC "[31m"

#endif // BXR_DEFS_H_
