/**
 * @file game.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef GAME_H_
#define GAME_H_

#include <boxer/bxr.h>

#include "animation.h"
#include "camera.h"
#include "map.h"

typedef struct game_s
{
  bxr_ecs_t *ecs;
  camera_t *camera;
  map_t *map;
  double delta_ms; // time in seconds since last update
  double alpha_ms; // time in seconds since last render, used for interpolation

  animation_t *player_animation;

  bxr_ecs_entity_t player_entity;
} game_t;

/**
 * Creates a new game instance.
 *
 * `map` is a pointer to the map to use for the game.
 *
 * `spawn_position` is the initial position of the player in world space.
 *
 * `return` a pointer to the newly created game instance, or NULL on failure.
 */
game_t *game_create(map_t *map, bxr_vec2_t spawn_position);

/**
 * Updates the game.
 *
 * `game` is a pointer to the game instance to update.
 *
 * `delta_ms` is the time in milliseconds since the last update.
 */
void game_update(game_t *game, double delta_ms);

/**
 * Renders the game.
 *
 * `game` is a pointer to the game instance to render.
 *
 * `alpha_ms` is the time in milliseconds since the last render, used for
 * interpolation.
 */
void game_render(game_t *game, double alpha_ms);

/**
 * Destroys the game instance and releases all associated resources.
 *
 * `game` is a pointer to the game instance to destroy.
 */
void game_destroy(game_t *game);

#endif // GAME_H_
