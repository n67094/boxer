#include <boxer/bxr.h>

#include "components.h"
#include "factory.h"
#include "player.h"

bxr_ecs_entity_t
factory_player_create(bxr_ecs_t *ecs, factory_player_desc_t *desc)
{
  BXR_ASSERT(ecs != NULL && "ecs must not be NULL");

  bxr_ecs_entity_t entity = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(
      ecs, entity, component_velocity, &(bxr_vec2_t){ 0.0f, 0.0f });
  bxr_ecs_entity_add_component(ecs, entity, component_gravity, NULL);

  bxr_ecs_entity_add_component(
      ecs, entity, component_position, &desc->position);
  bxr_ecs_entity_add_component(
      ecs, entity, component_collider, &desc->collider);
  bxr_ecs_entity_add_component(
      ecs, entity, component_animation, &desc->animation);

  bxr_ecs_entity_add_component(ecs,
                               entity,
                               component_player,
                               &(player_t){ .state     = STATE_IDLE,
                                            .direction = DIR_RIGHT,
                                            .speed     = 0.1f,
                                            .on_ground = false });

  return entity;
}
