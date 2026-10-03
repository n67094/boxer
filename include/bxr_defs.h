/**
 * File `bxr_defs.h`.
 *
 * Common macros shared by every boxer module.
 *
 * Provides symbol export (`BXR_API`), forced inlining (`BXR_INLINE`),
 * initialization cookies, small utility macros and ANSI terminal escape codes.
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
 *
 * FIXME: on Windows a static build still gets `dllimport` in client code; add a
 * `BOXER_STATIC` switch that expands to nothing.
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

/**
 * Declares a header function as `static inline` and forces it to be inlined.
 *
 * FIXME: the MSVC and fallback branches define `INLINE` instead of
 * `BXR_INLINE`, so every header using `BXR_INLINE` fails to compile there, and
 * `INLINE` clashes with the one defined in `SDL_gp.h`. Define `BXR_INLINE` as
 * `static __forceinline` (MSVC) and `static inline` (fallback) instead.
 * FIXME: `__attribute` is a GNU alternate spelling; prefer `__attribute__`.
 */
#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define BXR_INLINE static inline __attribute((always_inline))
#else
#define INLINE
#endif

/**
 * Organization name used to build the per-user preference directory.
 */
#define BXR_ORGANIZATION_NAME "boxer"

/**
 * Magic value stored by a subsystem once initialized, checked by its functions
 * with `BXR_ASSERT()`.
 */
#define BXR_INIT_COOKIE 0xC0DED1ED

/**
 * Substitutes a default for a zero value, typically an unset parameter.
 *
 * `val` value to check; evaluated twice, so it must have no side effects.
 * `def` value used when `val` is 0; evaluated only in that case.
 *
 * `return` `def` if `val` is 0, otherwise `val`.
 */
#define BXR_DEFAULT(val, def) (((val) == 0) ? (def) : (val))

/**
 * Gets the byte offset of a member within a struct.
 *
 * `TYPE` struct type.
 * `ELEMENT` member name.
 *
 * `return` the offset as a `size_t`.
 *
 * FIXME: dereferencing a null pointer is undefined behavior; use `offsetof()`
 * from `<stddef.h>` instead.
 */
#define BXR_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

/**
 * Gets the number of elements of a fixed-size array.
 *
 * `arr` array object; must not be a pointer (e.g. an array function parameter),
 * otherwise the result is silently wrong.
 *
 * `return` the element count as a `size_t`.
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
