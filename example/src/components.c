#include <boxer/bxr.h>

#include "components.h"

bxr_ecs_component_t component_position;
bxr_ecs_component_t component_velocity;
bxr_ecs_component_t component_sprite;
bxr_ecs_component_t component_player;

void
component_register(bxr_ecs_t *ecs)
{
  component_position
      = bxr_ecs_component_define(ecs, sizeof(bxr_vec2_t), NULL, NULL);
  component_sprite
      = bxr_ecs_component_define(ecs, sizeof(sprite_t), NULL, NULL);
  component_player = bxr_ecs_component_define(ecs, sizeof(bool), NULL, NULL);
}
