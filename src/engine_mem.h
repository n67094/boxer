/**
 * @file engine_mem.h
 *
 * @brief Memory allocation macros.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_MEM_H_
#define ENGINE_MEM_H_

#define ENGINE_ALLOC(ptr, nbytes) ((ptr) = SDL_malloc(nbytes))

#define ENGINE_CALLOC(ptr, count, nbytes)                                      \
  ((ptr) = SDL_calloc((size_t)(count), (size_t)(nbytes)))

#define ENGINE_NEW(ptr) ENGINE_ALLOC((ptr), (long)sizeof *(ptr))

#define ENGINE_NEWO(ptr) ENGINE_CALLOC((ptr), 1, (long)sizeof *(ptr))

#define ENGINE_FREE(ptr) ((void)(SDL_free(ptr), (ptr) = NULL))

#define ENGINE_RESIZE(ptr, nbytes)                                             \
  ((ptr) = SDL_realloc((ptr), (size_t)(nbytes)))

#endif // ENGINE_MEM_H_
