#ifndef FACTORY_H_
#define FACTORY_H_

#include <boxer/bxr.h>

#include "animation.h"

/**
 * Creates a new player entity.
 *
 * `ecs` is a pointer to the ECS where the player entity will be created.
 *
 * `spawn_position` is the initial position of the player in world space.
 *
 * `animation` is a pointer to the animation data for the player.
 *
 * `return` the entity ID of the newly created player entity.
 */
bxr_ecs_entity_t factory_player_create(bxr_ecs_t *ecs,
                                       bxr_vec2_t spawn_position,
                                       animation_t *animation);

#endif // FACTORY_H_
