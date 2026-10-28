#include <boxer/bxr.h>

#include "animation.h"
#include "components.h"
#include "game.h"
#include "player.h"

bxr_ecs_component_t component_position;
bxr_ecs_component_t component_velocity;
bxr_ecs_component_t component_collider;
bxr_ecs_component_t component_gravity;
bxr_ecs_component_t component_animation;
bxr_ecs_component_t component_player;

void
components_register(game_t *game)
{
  bxr_ecs_t *ecs = game->ecs;

  component_position
      = bxr_ecs_component_define(ecs, sizeof(bxr_vec2_t), NULL, NULL);
  component_velocity
      = bxr_ecs_component_define(ecs, sizeof(bxr_vec2_t), NULL, NULL);
  component_collider
      = bxr_ecs_component_define(ecs, sizeof(bxr_rect_t), NULL, NULL);
  component_gravity = bxr_ecs_component_define(ecs, sizeof(float), NULL, NULL);
  component_animation
      = bxr_ecs_component_define(ecs, sizeof(animation_t), NULL, NULL);
  component_player
      = bxr_ecs_component_define(ecs, sizeof(player_t), NULL, NULL);
}
