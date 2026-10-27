#include <boxer/bxr.h>

#include "components.h"
#include "factory.h"
#include "types.h"

bxr_ecs_entity_t
factory_create_player(bxr_ecs_t *ecs, bxr_vec2_t position)
{
  BXR_ASSERT(ecs != NULL && "ecs must not be NULL");

  bxr_ecs_entity_t entity = bxr_ecs_entity_create(ecs);

  // The image is keep until the game exits
  bxr_image_t player_image = bxr_image_create("assets/player.png");

  bxr_ecs_entity_add_component(ecs, entity, component_position, &position);
  bxr_ecs_entity_add_component(
      ecs, entity, component_velocity, &(bxr_vec2_t){ 0.0f, 0.0f });
  bxr_ecs_entity_add_component(
      ecs, entity, component_collider, &(bxr_rect_t){ 0.0f, 0.0f, 8.0f, 8.0f });
  bxr_ecs_entity_add_component(ecs, entity, component_gravity, NULL);
  bxr_ecs_entity_add_component(ecs, entity, component_sprite, &player_image);
  bxr_ecs_entity_add_component(ecs,
                               entity,
                               component_player,
                               &(player_t){ .state     = STATE_IDLE,
                                            .direction = DIR_RIGHT,
                                            .speed     = 0.1f,
                                            .on_ground = false });

  return entity;
}
