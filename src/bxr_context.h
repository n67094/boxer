/**
 * File `bxr_context.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Boxer context
 *
 * TODO improve context API, add more functions to set/get context state (e.g.
 * window title, icon, etc.).
 */

#ifndef BXR_CONTEXT_H_
#define BXR_CONTEXT_H_

#include <SDL3/SDL.h>

#include "bxr_game.h"
#include "bxr_math.h"

typedef struct bxr_context_s
{
  Uint32 _initialized;

  bxr_game_config_t config;

  SDL_GPUDevice *gpu_device;
  SDL_AudioDeviceID *audio_devices;
  SDL_Window *window;
  SDL_Event *event;

  Uint64 last_time_ms;
  Uint64 lag_ms;
  Uint64 delta_ms;
  Uint64 alpha_ms;

  int fps;
  int ups;

  SDL_GPUTexture *target_texture;
  SDL_GPUTextureFormat texture_format;
  SDL_PixelFormat pixel_format;
  SDL_GPUCommandBuffer *cmd_buffer;
} bxr_context_t;

/**
 * Get the global context instance.
 *
 * `return` Pointer to the global context instance.
 */
bxr_context_t *bxr_context_get(void);

/**
 * Check if the global context is valid (initialized).
 *
 * `return` `true` if the context is valid, `false` otherwise.
 */
bool bxr_context_is_valid(void);

/**
 * Set the window dimensions.
 *
 * `width`  The new width of the window in pixels.
 * `height` The new height of the window in pixels.
 */
void bxr_context_set_window_dimensions(int width, int height);

/**
 * Get the current window dimensions.
 *
 * `return` The current window dimensions as a `bxr_vec2_t` (x = width, y =
 * height).
 */
bxr_vec2_t bxr_context_get_window_dimensions(void);

/**
 * Set the fullscreen mode of the window.
 *
 * `fullscreen` `true` to set the window to fullscreen mode, `false` to set it
 * to windowed mode.
 */
void bxr_context_set_fullscreen(bool fullscreen);

/**
 * Check if the window is currently in fullscreen mode.
 *
 * `return` `true` if the window is in fullscreen mode, `false` otherwise.
 */
bool bxr_context_is_fullscreen(void);

/**
 * Set the resizable property of the window.
 *
 * `resizable` `true` to make the window resizable, `false` to make it
 * non-resizable.
 */
void bxr_context_set_resizable(bool resizable);

/**
 * Check if the window is currently resizable.
 *
 * `return` `true` if the window is resizable, `false` otherwise.
 */
bool bxr_context_is_resizable(void);

#endif // BXR_CONTEXT_H_
