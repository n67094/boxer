/**
 * @file game.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef GAME_H_
#define GAME_H_

#include <boxer/bxr.h>

#include "map.h"

typedef struct game_s
{
  bxr_ecs_t *ecs;
  map_t *map;
  double delta_ms; // time in seconds since last update
  double alpha_ms; // time in seconds since last render, used for interpolation
} game_t;

void game_setup(void);

void game_set_map(map_t *map);

void game_update(double delta_ms);

void game_render(double alpha_ms);

void game_teardown(void);

#endif // GAME_H_
