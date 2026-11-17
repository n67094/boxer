/**
 * File `bxr_main.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Program Entry Point
 *
 * Defines the program entry point (through SDL main callbacks) and connects it
 * to the `bxr_game_*()` callbacks from `bxr_game.h`.
 *
 * Include it in exactly one source file of your game, usually the one that
 * implements the `bxr_game_*()` callbacks:
 *
 * ```cpp
 * #include <boxer/bxr.h>
 * #include <boxer/bxr_main.h>
 * ```
 *
 * The entry point lives in the game executable rather than in the library, so
 * boxer can be linked either statically or as a shared library (`.so`,
 * `.dylib` or `.dll`).
 *
 * **NOTE:** like `SDL_main.h`, which it includes, it defines functions, so
 * including it in more than one source file fails to link.
 */

#ifndef BXR_MAIN_H_
#define BXR_MAIN_H_

#include <SDL3/SDL.h>

#include "bxr.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char **argv)
{
  static const bxr_game_callbacks_t game = {
    .config   = bxr_game_config,
    .setup    = bxr_game_setup,
    .update   = bxr_game_update,
    .render   = bxr_game_render,
    .teardown = bxr_game_teardown,
  };

  (void)appstate;

  return bxr_app_init(&game, argc, argv);
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
  (void)appstate;

  return bxr_app_iterate();
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
  (void)appstate;

  return bxr_app_event(event);
}

void
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  (void)appstate;

  bxr_app_quit(result);
}

#endif // BXR_MAIN_H_
