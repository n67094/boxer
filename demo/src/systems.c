#include "systems.h"
#include "animation.h"
#include "components.h"
#include "defs.h"
#include "game.h"

bxr_ecs_system_t system_player_update;
bxr_ecs_system_t system_animation_update;
bxr_ecs_system_t system_animation_render;
bxr_ecs_system_t system_map_render;

int
system_player_update_cb(bxr_ecs_t *ecs,
                        const bxr_ecs_entity_t *entities,
                        size_t entity_count,
                        void *udata)
{
  // TODO handle player input and update player state

  return 0;
}

int
system_animation_update_cb(bxr_ecs_t *ecs,
                           const bxr_ecs_entity_t *entities,
                           size_t entity_count,
                           void *udata)
{
  game_t *game = (game_t *)udata;

  for (size_t i = 0; i < entity_count; ++i) {
    bxr_ecs_entity_t entity = entities[i];

    animation_t *animation
        = bxr_ecs_entity_get_component(ecs, entity, component_animation);

    if (animation) {
      animation_update(animation, game->delta_ms);
    }
  }

  return 0;
}

int
system_animation_render_cb(bxr_ecs_t *ecs,
                           const bxr_ecs_entity_t *entities,
                           size_t entity_count,
                           void *udata)
{
  game_t *game        = (game_t *)udata;
  bxr_rect_t viewport = camera_get_viewport(game->camera);

  for (size_t i = 0; i < entity_count; ++i) {
    bxr_ecs_entity_t entity = entities[i];

    animation_t *animation
        = bxr_ecs_entity_get_component(ecs, entity, component_animation);
    bxr_vec2_t *position
        = bxr_ecs_entity_get_component(ecs, entity, component_position);

    if (animation && position) {
      animation_render(
          animation,
          (bxr_vec2_t){ position->x - viewport.x, position->y - viewport.y });
    }
  }

  return 0;
}

int
system_map_render_cb(bxr_ecs_t *ecs,
                     const bxr_ecs_entity_t *entities,
                     size_t entity_count,
                     void *udata)
{
  game_t *game = (game_t *)udata;

  map_render(game->map, camera_get_viewport(game->camera));

  return 0;
}

void
systems_register(game_t *game)
{
  BXR_ASSERT(game != NULL && "game must not be NULL");

  bxr_ecs_t *ecs = game->ecs;

  system_player_update = bxr_ecs_system_define(
      ecs, 0, system_player_update_cb, NULL, NULL, game);
  system_animation_update = bxr_ecs_system_define(
      ecs, 0, system_animation_update_cb, NULL, NULL, game);
  system_animation_render = bxr_ecs_system_define(
      ecs, 0, system_animation_render_cb, NULL, NULL, game);
  system_map_render
      = bxr_ecs_system_define(ecs, 0, system_map_render_cb, NULL, NULL, game);
}
