/**
 * File `bxr_mem.h`.
 *
 * Memroy allocation and memory manipulation utilities.
 *
 * Allocation macros assign the result to `ptr` directly, so `ptr` must be a
 * modifiable lvalue. Every allocation must be released with `BXR_FREE()`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_MEM_H_
#define BXR_MEM_H_

#include <SDL3/SDL.h>

/**
 * Allocate uninitialized memory and assign it to `ptr`.
 *
 * `ptr` lvalue receiving the allocated block.
 * `nbytes` size of the block in bytes.
 *
 * `return` the new pointer (also stored in `ptr`), or NULL on failure.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 */
#define BXR_ALLOC(ptr, nbytes) ((ptr) = SDL_malloc((size_t)nbytes))

/**
 * Allocate zero-initialized memory for an array and assign it to `ptr`.
 *
 * `ptr` lvalue receiving the allocated block.
 * `count` number of elements.
 * `nbytes` size of one element in bytes.
 *
 * `return` the new pointer (also stored in `ptr`), or NULL on failure.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 */
#define BXR_CALLOC(ptr, count, nbytes)                                         \
  ((ptr) = SDL_calloc((size_t)(count), (size_t)(nbytes)))

/**
 * Allocate one uninitialized object of the type pointed to by `ptr`.
 *
 * `ptr` typed pointer lvalue receiving the allocated object.
 *
 * `return` the new pointer (also stored in `ptr`), or NULL on failure.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 */
#define BXR_NEW(ptr) BXR_ALLOC((ptr), sizeof *(ptr))

/**
 * Allocate one zero-initialized object of the type pointed to by `ptr`.
 *
 * `ptr` typed pointer lvalue receiving the allocated object.
 *
 * `return` the new pointer (also stored in `ptr`), or NULL on failure.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 */
#define BXR_NEW0(ptr) BXR_CALLOC((ptr), 1, sizeof *(ptr))

/**
 * Free memory and set `ptr` to NULL.
 *
 * `ptr` lvalue pointing to memory from the BXR allocation macros, NULL is
 * allowed.
 */
#define BXR_FREE(ptr) ((void)(SDL_free(ptr), (ptr) = NULL))

/**
 * Resize a block of memory and assign the new pointer to `ptr`.
 *
 * `ptr` lvalue pointing to the block to resize, NULL behaves like
 * `BXR_ALLOC()`.
 * `nbytes` new size of the block in bytes.
 *
 * `return` the new pointer (also stored in `ptr`), or NULL on failure.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 *
 * FIXME: on failure `ptr` is overwritten with NULL and the original block is
 * leaked. Realloc into a temporary and only assign it to `ptr` on success
 * (callers in `bxr_ecs.c`, `bxr_multimap.c` and `bxr_sparse_set.c` also never
 * check the result).
 */
#define BXR_RESIZE(ptr, nbytes) ((ptr) = SDL_realloc((ptr), (size_t)(nbytes)))

/**
 * Fill `nbytes` of memory at `ptr` with the byte `value`.
 *
 * `return` `ptr`.
 */
#define BXR_MEMSET(ptr, value, nbytes)                                         \
  SDL_memset((ptr), (value), (size_t)(nbytes))

/**
 * Copy `nbytes` from `src` to `dest`, the regions must not overlap.
 *
 * `return` `dest`.
 */
#define BXR_MEMCPY(dest, src, nbytes)                                          \
  SDL_memcpy((dest), (src), (size_t)(nbytes))

/**
 * Copy `nbytes` from `src` to `dest`, the regions may overlap.
 *
 * `return` `dest`.
 */
#define BXR_MEMMOVE(dest, src, nbytes)                                         \
  SDL_memmove((dest), (src), (size_t)(nbytes))

/**
 * Compare the first `nbytes` of two memory regions.
 *
 * `return` 0 if equal, a negative or positive value if the first differing
 * byte in `ptr1` is lower or greater than the one in `ptr2`.
 */
#define BXR_MEMCMP(ptr1, ptr2, nbytes)                                         \
  SDL_memcmp((ptr1), (ptr2), (size_t)(nbytes))

#endif // BXR_MEM_H_
