/**
 * File `bxr_context_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Context (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_CONTEXT_INTERNAL_H_
#define BXR_CONTEXT_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"
#include "bxr_game.h"

/**
 * ## Typedefs
 */

/**
 * Global context of the application.
 *
 * Contains the configuration, the window, the GPU device and other global
 * state.
 */
typedef struct bxr_context_s
{
  bxr_game_config_t config;

  SDL_Window *window;
  SDL_PixelFormat pixel_format;
  SDL_GPUTextureFormat texture_format;
  SDL_GPUDevice *gpu_device;

  Uint64 target_delta_ns; // fixed time between updates, never 0.
  Uint64 frame_count; // number of frames rendered since the application started
} bxr_context_t;

/**
 * Get the global context and create it if it doesn't exist yet.
 *
 * `return` the global context, or `NULL` if the allocation failed.
 *
 * **Note**: the context is owned by the engine, the caller must not free it.
 */
bxr_context_t *bxr_context_get(void);

#endif // BXR_CONTEXT_INTERNAL_H_
