#ifndef FACTORY_H_
#define FACTORY_H_

#include <boxer/bxr.h>

#include "animation.h"

bxr_ecs_entity_t factory_player_create(bxr_ecs_t *ecs,
                                       bxr_vec2_t spawn_position,
                                       animation_t *animation);

#endif // FACTORY_H_
