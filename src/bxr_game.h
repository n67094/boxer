/**
 * @file bxr_game.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_GAME_H_
#define BXR_GAME_H_

#include <SDL3/SDL.h>

typedef struct bxr_game_config_s
{
  const char *name; // application name
  const char *title;

  unsigned int width;
  unsigned int height;

  bool fullscreen;
  bool resizable;

  unsigned int target_ups;
} bxr_game_config_t;

/**
 * @brief The user should provide this function to return the bxr game
 * configuration.
 *
 * @return A pointer to the bxr DEFAULT game configuration structure.
 *
 * @note If a file named "config.ini" exists in the PHYSFS mount directories,
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
 * @brief The user should provide this function to setup the game.
 *
 * @details This function is called once at the start of the game, after the
 * bxr has been initialized and the window has been created. You can use this
 * function to load assets, initialize game state, etc.
 */
extern void bxr_game_setup(void);

/**
 * @brief The user should provide this function to update the game logic.
 *
 * @details This function is called every frame with a fixed time step
 * (delta_time_ms) that represents the time elapsed since the last update. You
 * should use this function to update your game logic, such as moving objects,
 * handling input, etc.
 */
extern void bxr_game_update(Uint64 delta_time_ms);

/**
 * @brief The user should provide this function to render the game.
 *
 * @details This function is called every frame after the update function, and
 * it is where you should put your rendering code. The alpha_time_ms parameter
 * is an interpolation factor that can be used to smooth out rendering between
 * updates, especially if your update rate is lower than your frame rate.
 */
extern void bxr_game_render(Uint64 alpha_time_ms);

/**
 * @brief The user should provide this function to shutdown the game.
 *
 * @details This function is called once when the game is shutting down, after
 * the main loop has exited. You can use this function to free resources, save
 * game state, etc.
 */
extern void bxr_game_shutdown(void);

#endif // BXR_GAME_H_
