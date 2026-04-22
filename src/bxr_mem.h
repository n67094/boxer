/**
 * File `bxr_mem.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Memory allocation macros
 */

#ifndef BXR_MEM_H_
#define BXR_MEM_H_

#define BXR_ALLOC(ptr, nbytes) ((ptr) = SDL_malloc(nbytes))

#define BXR_CALLOC(ptr, count, nbytes)                                         \
  ((ptr) = SDL_calloc((size_t)(count), (size_t)(nbytes)))

#define BXR_NEW(ptr) BXR_ALLOC((ptr), (long)sizeof *(ptr))

#define BXR_NEWO(ptr) BXR_CALLOC((ptr), 1, (long)sizeof *(ptr))

#define BXR_FREE(ptr) ((void)(SDL_free(ptr), (ptr) = NULL))

#define BXR_RESIZE(ptr, nbytes) ((ptr) = SDL_realloc((ptr), (size_t)(nbytes)))

#endif // BXR_MEM_H_
