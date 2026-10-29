/**
 * File `systems.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#include <boxer/bxr.h>

#include "game.h"

extern bxr_ecs_system_t system_player_update;
extern bxr_ecs_system_t system_animation_update;
extern bxr_ecs_system_t system_animation_render;
extern bxr_ecs_system_t system_map_render;

void systems_register(game_t *game);

#endif // SYSTEMS_H_
