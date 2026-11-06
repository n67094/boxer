/**
 * File `bxr_game.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Game Lifecycle
 *
 * Boxer owns `main()`. Don't implement it yourself. Use the five callbacks
 * below to implement your game logic:
 *
 * - `bxr_game_config()`
 * - `bxr_game_setup()`
 * - `bxr_game_update()`
 * - `bxr_game_render()`
 * - `bxr_game_teardown()`
 *
 * Typical usage:
 *
 * ```cpp
 * static bxr_game_config_t _config = { 0 };
 *
 * const bxr_game_config_t *bxr_game_config(void) {
 *   _config.name           = "my_game";
 *   _config.title          = "My Game";
 *
 *   _config.width          = 960;
 *   _config.height         = 540;
 *
 *   _config.virtual_width  = 320;
 *   _config.virtual_height = 180;
 *
 *   return &_config;
 * }
 *
 * void bxr_game_setup(void) { ... }          // load assets, create state
 *
 * void bxr_game_update(double delta_ms) { ... }
 *
 * void bxr_game_render(double alpha_ms) {
 *   bxr_painter_set_color(BXR_COLOR_BLACK);
 *   bxr_painter_clear();
 *   ...
 * }
 *
 * void bxr_game_teardown(void) { ... }       // release assets, destroy state
 * ```
 */

#ifndef BXR_GAME_H_
#define BXR_GAME_H_

#include <SDL3/SDL.h>

/**
 * ## Typedefs
 */

/**
 * Boxer configuration.
 *
 * `name` application name, also used to build the save directory
 * (e.g. `~/.local/share/<name>/` on Linux). Must not be `NULL` and must stay
 * valid until exit since only the pointer is kept (e.g. a string literal).
 *
 * `title` window title. Must not be `NULL` and must stay valid until exit
 * (e.g. a string literal).
 *
 * `width` and `height` initial window size in window coordinates, > 0.
 *
 * `virtual_width` and `virtual_height` resolution the game draws in, > 0 and
 * not larger than `width` and `height`.
 *
 * `fullscreen` create the window in fullscreen mode.
 *
 * `resizable` allow the user to resize the window.
 */
typedef struct bxr_game_config_s
{
  const char *name;
  const char *title;

  unsigned int width;
  unsigned int height;

  unsigned int virtual_width;
  unsigned int virtual_height;

  bool fullscreen;
  bool resizable;
} bxr_game_config_t;

/**
 * ## API
 */

/**
 * Provide the game configuration, implemented by the user.
 *
 * Called once at startup, before SDL and any subsystem are initialized, so it
 * must not call any boxer function.
 *
 * `return` the configuration, must not be `NULL`. Its fields are copied right
 * after the call, so a `static` variable is enough and nothing is freed.
 */
extern const bxr_game_config_t *bxr_game_config(void);

/**
 * Initialize the game state, implemented by the user.
 *
 * Called once after every boxer subsystem are ready, so assets, audio and input
 * can be used here.
 */
extern void bxr_game_setup(void);

/**
 * Advance the game logic by one fixed step, implemented by the user.
 *
 * Called zero or more times per frame, at a fixed rate matching the display
 * refresh rate (60 Hz if it can't be queried). When the game falls behind,
 * at most 5 steps are caught up and the remaining lag is dropped.
 *
 * `delta_ms` duration of the fixed step in milliseconds (e.g. 16.67 at
 * 60 Hz), the same value on every call until the refresh rate changes.
 *
 * **NOTE:** Input state is refreshed before each step.
 */
extern void bxr_game_update(double delta_ms);

/**
 * Draw the current game state, implemented by the user.
 *
 * Called once per frame after the updates. Drawing
 * coordinates are in the virtual resolution from the configuration.
 *
 * `alpha_ms` how far the current time is between the last update and the next
 * one, in [0, 1), used to interpolate between the previous and current update
 * state for smooth rendering.
 */
extern void bxr_game_render(double alpha_ms);

/**
 * Release the game resources, implemented by the user.
 *
 * Called once on exit, before boxer subsystems are torn down, so they are
 * still usable here.
 *
 * **NOTE:** this is also called when engine startup failed, possibly before
 * `bxr_game_setup()` ran, so it must handle a partially or not initialized
 * state.
 */
extern void bxr_game_teardown(void);

#endif // BXR_GAME_H_
