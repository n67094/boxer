#include "systems.h"
#include "animation.h"
#include "components.h"
#include "game.h"
#include "player.h"

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
  game_t *game = (game_t *)udata;

  for (size_t i = 0; i < entity_count; ++i) {
    bxr_ecs_entity_t entity = entities[i];

    player_t *player
        = bxr_ecs_entity_get_component(ecs, entity, component_player);
    bxr_vec2_t *position
        = bxr_ecs_entity_get_component(ecs, entity, component_position);
    bxr_vec2_t *velocity
        = bxr_ecs_entity_get_component(ecs, entity, component_velocity);
    bxr_rect_t *collider
        = bxr_ecs_entity_get_component(ecs, entity, component_collider);
    float *gravity
        = bxr_ecs_entity_get_component(ecs, entity, component_gravity);
    animation_t *animation
        = bxr_ecs_entity_get_component(ecs, entity, component_animation);

    if (!player || !position || !velocity || !collider || !gravity
        || !animation) {
      continue;
    }

    player_update(player, animation, velocity);

    // Apply gravity
    velocity->y += (*gravity) * game->delta_ms;
    if (velocity->y > PLAYER_MAX_FALL) {
      velocity->y = PLAYER_MAX_FALL;
    }

    // X axis
    bxr_vec2_t next_position = {
      .x = position->x + velocity->x * game->delta_ms,
      .y = position->y,
    };

    bxr_rect_t box = {
      .x = next_position.x + collider->x,
      .y = next_position.y + collider->y,
      .w = collider->w,
      .h = collider->h,
    };

    if (map_rect_is_solid(game->map, box)) {
      velocity->x = 0.0f; // Stop horizontal movement on collision
    }

    position->x += velocity->x * game->delta_ms;

    // Y axis
    next_position = (bxr_vec2_t){
      .x = position->x,
      .y = position->y + velocity->y * game->delta_ms,
    };

    box = (bxr_rect_t){
      .x = next_position.x + collider->x,
      .y = next_position.y + collider->y,
      .w = collider->w,
      .h = collider->h,
    };

    player->on_ground = false; // Reset on_ground before checking for collision
    if (map_rect_is_solid(game->map, box)) {
      if (velocity->y > 0.0f) {
        player->on_ground = true; // Only landing counts as ground
      }
      velocity->y = 0.0f; // Stop vertical movement on collision
    }

    position->y += velocity->y * game->delta_ms;
  }

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
      animation_render(animation,
                       (bxr_vec2_t){ bxr_floorf(position->x - viewport.x),
                                     bxr_floorf(position->y - viewport.y) });
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
