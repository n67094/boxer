#include <boxer/bxr.h>

#include "components.h"
#include "factory.h"
#include "player.h"

bxr_ecs_entity_t
factory_player_create(bxr_ecs_t *ecs,
                      bxr_vec2_t spawn_position,
                      animation_t *animation)
{
  BXR_ASSERT(ecs != NULL && "ecs must not be NULL");

  bxr_ecs_entity_t entity = bxr_ecs_entity_create(ecs);

  // Velocity
  bxr_ecs_entity_add_component(ecs, entity, component_velocity, NULL);
  bxr_vec2_t *velocity
      = bxr_ecs_entity_get_component(ecs, entity, component_velocity);
  velocity->x = 0.0f;
  velocity->y = 0.0f;

  // Gravity
  bxr_ecs_entity_add_component(ecs, entity, component_gravity, NULL);
  float *gravity = bxr_ecs_entity_get_component(ecs, entity, component_gravity);
  *gravity       = 1.0f;

  // Position
  bxr_ecs_entity_add_component(ecs, entity, component_position, NULL);
  bxr_vec2_t *pos
      = bxr_ecs_entity_get_component(ecs, entity, component_position);
  pos->x = spawn_position.x;
  pos->y = spawn_position.y;

  // Collider
  bxr_ecs_entity_add_component(ecs, entity, component_collider, NULL);
  bxr_rect_t *collider
      = bxr_ecs_entity_get_component(ecs, entity, component_collider);
  *collider = PLAYER_COLLIDER;

  // Animation
  bxr_ecs_entity_add_component(ecs, entity, component_animation, NULL);
  animation_t *anim
      = bxr_ecs_entity_get_component(ecs, entity, component_animation);
  *anim = *animation;

  // Player
  bxr_ecs_entity_add_component(ecs, entity, component_player, NULL);
  player_t *player
      = bxr_ecs_entity_get_component(ecs, entity, component_player);
  *player = PLAYER_DEFAULT;

  return entity;
}
