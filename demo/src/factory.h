#ifndef FACTORY_H_
#define FACTORY_H_

#include <boxer/bxr.h>

#include "animation.h"

typedef struct factory_player_desc_s {
  bxr_vec2_t position;
  bxr_rect_t collider;
  animation_t animation;
} factory_player_desc_t;

bxr_ecs_entity_t factory_player_create(bxr_ecs_t *ecs, factory_player_desc_t *desc);

#endif // FACTORY_H_
