/**
 * File `bxr_context.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Global Context
 */

#ifndef BXR_CONTEXT_H_
#define BXR_CONTEXT_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_game.h"
#include "bxr_math.h"

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
BXR_API bxr_context_t *bxr_context_get(void);

/**
 * Get the GPU device used for rendering.
 *
 * `return` the GPU device, or `NULL` if not created yet.
 *
 * **Note**: the device is owned by the engine, the caller must not destroy it.
 */
BXR_API SDL_GPUDevice *bxr_context_get_gpu_device(void);

/**
 * Get the game window.
 *
 * `return` the window, or `NULL` if not created yet.
 *
 * **Note**: the window is owned by the engine, the caller must not destroy it.
 */
BXR_API SDL_Window *bxr_context_get_window(void);

/**
 * Resize the window and store the new size in the configuration.
 *
 * `width` new width in window coordinates.
 * `height` new height in window coordinates.
 *
 * return `true` if the window was resized successfully, `false` otherwise.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool bxr_context_set_window_dimensions(int width, int height);

/**
 * Get the window size stored in the configuration.
 *
 * `return` the size in window coordinates (x = width, y = height).
 */
BXR_API bxr_vec2_t bxr_context_get_window_dimensions(void);

/**
 * Set the virtual resolution the game renders at.
 *
 * Takes effect on the next frame, the window itself is not resized.
 *
 * `width` virtual width in pixels.
 * `height` virtual height in pixels.
 */
BXR_API void bxr_context_set_virtual_window_dimensions(int width, int height);

/**
 * Get the virtual resolution the game renders at.
 *
 * `return` the virtual size in pixels (x = width, y = height).
 */
BXR_API bxr_vec2_t bxr_context_get_virtual_window_dimensions(void);

/**
 * Switch the window between fullscreen and windowed mode.
 *
 * `fullscreen` `true` for fullscreen, `false` for windowed.
 *
 * `return` true on success, false on failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool bxr_context_set_fullscreen(bool fullscreen);

/**
 * Check whether the window is in fullscreen mode.
 *
 * `return` true if fullscreen, `false` otherwise.
 */
BXR_API bool bxr_context_is_fullscreen(void);

/**
 * Allow or forbid the user to resize the window.
 *
 * `resizable` `true` to allow resizing, `false` to forbid it.
 *
 * `return` true on success, false on failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool bxr_context_set_resizable(bool resizable);

/**
 * Check whether the window can be resized.
 *
 * `return` true if the window is resizable, `false` otherwise.
 */
BXR_API bool bxr_context_is_resizable(void);

/**
 * Get the number of frames rendered since the application started.
 *
 * `return` the frame count.
 */
BXR_API Uint64 bxr_context_get_frame_count(void);

#endif // BXR_CONTEXT_H_
