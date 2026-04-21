/**
 * TODO refactor documentation
 *
 * file `bxr_game.h`
 *
 * copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Game lifecycle management.
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

  bool fullscreen;
  bool resizable;

  unsigned int target_ups;
} bxr_game_config_t;

/**
 * TODO check the pertinence of this ?
 *
 * The user should provide this function to return the bxr game
 * configuration.
 *
 * `return` A pointer to the bxr DEFAULT game configuration structure.
 *
 * `note` If a file named "config.ini" exists in the PHYSFS mount directories,
 * the bxr will override the default configuration values with the values
 * from the INI file.
 *
 * Only those field will be overriden:
 *
 * - width
 * - height
 * - fullscreen
 * - resizable
 * - target_ups
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
 */
extern void bxr_game_update(Uint64 delta_time_ms);

/**
 * The user should provide this function to render the game.
 *
 * `alpha_time_ms` is the time in milliseconds since the last update call.
 */
extern void bxr_game_render(Uint64 alpha_time_ms);

/**
 * The user should provide this function to shutdown the game.
 */
extern void bxr_game_shutdown(void);

#endif // BXR_GAME_H_
