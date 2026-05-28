/**
 * File `bxr_game.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Game lifecycle management
 */

#ifndef BXR_GAME_H_
#define BXR_GAME_H_

#include <SDL3/SDL.h>

typedef struct bxr_game_config_s
{
  const char *name;  // application name
  const char *title; // window title

  unsigned int width;
  unsigned int height;

  unsigned int virtual_width;
  unsigned int virtual_height;

  bool fullscreen;
  bool resizable;
} bxr_game_config_t;

/**
 * The user should provide this function to return boxer's game configuration.
 *
 * `return` the game configuration to use for the game.
 */
extern bxr_game_config_t *bxr_game_config(void);

/**
 * The user should provide this function to setup the game.
 */
extern void bxr_game_setup(void);

/**
 * The user should provide this function to update the game logic.
 *
 * `delta_time_ms` is the time in milliseconds since the last update call.
 *
 * **NOTE:** The game is updated at a fixed rate defined by the `target_ups`
 * configuration value.
 */
extern void bxr_game_update(Uint64 delta_time_ms);

/**
 * The user should provide this function to render the game.
 *
 * `alpha_time_ms` is the time in milliseconds since the last update call.
 *
 * **NOTE:** The game is rendered at a variable rate, and `alpha_time_ms` can be
 * used to interpolate the game state for smooth rendering.
 */
extern void bxr_game_render(Uint64 alpha_time_ms);

/**
 * The user should provide this function to tear down the game.
 */
extern void bxr_game_teardown(void);

#endif // BXR_GAME_H_
