/**
 * File `bxr.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Main Header
 *
 * Boxer is a lightweight 2D game framework built for rapid development of
 * simple games. It offers a straightforward API for graphics, input, and core
 * game mechanics, which makes it a great fit for beginners and hobbyists.
 */

#ifndef BXR_H_
#define BXR_H_

#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_audio.h"
#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_error.h"
#include "bxr_game.h"
#include "bxr_gamepad.h"
#include "bxr_image.h"
#include "bxr_ini.h"
#include "bxr_keyboard.h"
#include "bxr_log.h"
#include "bxr_math.h"
#include "bxr_mem.h"
#include "bxr_mouse.h"
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_pixel.h"
#include "bxr_sampler.h"
#include "bxr_shader.h"
#include "bxr_string.h"
#include "bxr_time.h"
#include "bxr_unit.h"

/**
 * ## API
 */

/**
 * Request the application to exit.
 */
BXR_API void bxr_quit(void);

/**
 * ## Entry Point
 */

typedef struct bxr_game_callbacks_s
{
  const bxr_game_config_t *(*config)(void);
  void (*setup)(void);
  void (*update)(double delta_ms);
  void (*render)(double alpha_ms);
  void (*teardown)(void);
} bxr_game_callbacks_t;

/**
 * Start boxer and call the game setup, from `SDL_AppInit()`.
 *
 * `game` the game callbacks, all must be set. The struct is copied.
 *
 * `argc` and `argv` the program arguments.
 *
 * `return` `SDL_APP_CONTINUE` on success, `SDL_APP_FAILURE` otherwise.
 */
BXR_API SDL_AppResult bxr_app_init(const bxr_game_callbacks_t *game,
                                   int argc,
                                   char **argv);

/**
 * Run one frame (updates and render), from `SDL_AppIterate()`.
 */
BXR_API SDL_AppResult bxr_app_iterate(void);

/**
 * Handle one SDL event, from `SDL_AppEvent()`.
 */
BXR_API SDL_AppResult bxr_app_event(SDL_Event *event);

/**
 * Call the game teardown and stop boxer, from `SDL_AppQuit()`.
 */
BXR_API void bxr_app_quit(SDL_AppResult result);

#endif // BXR_H_
