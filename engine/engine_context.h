/**
 * @file engine_context.h
 *
 * @brief Engine context.
 *
 * @details The context structure is used to hold the global state of the
 * engine
 *
 * @note Subsystems will keep a reference `setup(&ctx)` for direct
 * access, but beware that some context member need side effects or validation,
 * so it's best to use this API whenever possible.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_CONTEXT_H_
#define ENGINE_CONTEXT_H_

#include <SDL3/SDL.h>

#include "engine_game.h"
#include "engine_math.h"

typedef struct engine_context_s
{
  Uint32 _initialized;

  engine_game_config_t config; // Copy of the user configuration

  SDL_GPUDevice *device;
  SDL_Window *window;
  SDL_Event *event;

  Uint64 last_time_ms;
  Uint64 lag_ms;   // Accumulated lag time
  Uint64 delta_ms; // Update at fixed time
  Uint64 alpha_ms; // Render interpolation factor

  int fps;
  int ups;

  SDL_GPUTexture *target_texture; // The current render target texture, used for
                                  // painter and image creation
  SDL_GPUTextureFormat texture_format; // The format of the target texture, used
                                       // for painter and image creation
  SDL_PixelFormat pixel_format; // The pixel format of the target texture, used
                                // for image loading and saving
  SDL_GPUCommandBuffer *cmd_buffer; // The current GPU command buffer, used for
                                    // painter and other GPU operations
} engine_context_t;

engine_context_t *engine_context_get(void);
bool engine_context_is_valid(void);

void engine_context_set_window_size(int width, int height);
engine_vec2_t engine_context_get_window_size(void);

void engine_context_set_fullscreen(bool fullscreen);
bool engine_context_is_fullscreen(void);

void engine_context_set_resizable(bool resizable);
bool engine_context_is_resizable(void);

#endif // ENGINE_CONTEXT_H_
