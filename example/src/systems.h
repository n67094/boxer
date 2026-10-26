/**
 * File `systems.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#include <boxer/bxr.h>

#include "state.h"

extern bxr_ecs_system_t system_player;
extern bxr_ecs_system_t system_map;

void system_register(state_t *game);

#endif // SYSTEMS_H_
