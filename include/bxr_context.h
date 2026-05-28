/**
 * File `bxr_context.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Boxer context
 *
 * The context module provides a global context for the game, which holds
 * important information about the engine state and configuration.
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

  Uint64 fps; // Frames per second (rendering fixed timestep)
  Uint64 ups; // Updates per second (game logic)

  float target_delta_ms;

  SDL_GPUDevice *gpu_device;
  SDL_AudioDeviceID *audio_devices;
  SDL_Window *window;
  SDL_GPUTextureFormat texture_format;
  SDL_PixelFormat pixel_format;
} bxr_context_t;

/**
 * Get the global context instance.
 *
 * `return` Pointer to the global context instance.
 */
BXR_API bxr_context_t *bxr_context_get(void);

/**
 * Set the window dimensions.
 *
 * `width`  The new width of the window in pixels.
 * `height` The new height of the window in pixels.
 */
BXR_API void bxr_context_set_window_dimensions(int width, int height);

/**
 * Get the current window dimensions.
 *
 * `return` The current window dimensions as a `bxr_vec2_t` (x = width, y =
 * height).
 */
BXR_API bxr_vec2_t bxr_context_get_window_dimensions(void);

/**
 * Set the virtual window dimensions.
 *
 * `width`  The new virtual width of the window in pixels.
 * `height` The new virtual height of the window in pixels.
 */
BXR_API void bxr_context_set_virtual_window_dimensions(int width, int height);

/**
 * Get the current virtual window dimensions.
 *
 * `return` The current virtual window dimensions as a `bxr_vec2_t` (x = width,
 * y = height).
 */
BXR_API bxr_vec2_t bxr_context_get_virtual_window_dimensions(void);

/**
 * Set the fullscreen mode of the window.
 *
 * `fullscreen` `true` to set the window to fullscreen mode, `false` to set it
 * to windowed mode.
 */
BXR_API void bxr_context_set_fullscreen(bool fullscreen);

/**
 * Check if the window is currently in fullscreen mode.
 *
 * `return` `true` if the window is in fullscreen mode, `false` otherwise.
 */
BXR_API bool bxr_context_is_fullscreen(void);

/**
 * Set the resizable property of the window.
 *
 * `resizable` `true` to make the window resizable, `false` to make it
 * non-resizable.
 */
BXR_API void bxr_context_set_resizable(bool resizable);

/**
 * Check if the window is currently resizable.
 *
 * `return` `true` if the window is resizable, `false` otherwise.
 */
BXR_API bool bxr_context_is_resizable(void);

#endif // BXR_CONTEXT_H_
