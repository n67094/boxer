/**
 * File `components.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <boxer/bxr.h>

extern bxr_ecs_component_t component_position;
extern bxr_ecs_component_t component_velocity;
extern bxr_ecs_component_t component_sprite;
extern bxr_ecs_component_t component_player;

typedef struct sprite_s
{
  bxr_image_t image;
} sprite_t;

typedef enum
{
  STATE_IDLE = 0,
  STATE_WALKING,
  STATE_JUMPING,
  STATE_FALLING,
} state_e;

typedef enum
{
  LEFT = 0,
  RIGHT,
} direction_e;

typedef struct
{
  state_e state;
  direction_e direction;
} player_t;

/**
 * Registers the components used in the ECS.
 *
 * `ecs` is a pointer to the ECS instance where the components will be
 * registered.
 */
void component_register(bxr_ecs_t *ecs);

#endif // COMPONENTS_H_
