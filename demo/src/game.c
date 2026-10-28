#include <boxer/bxr.h>

#include "components.h"
#include "defs.h"
#include "factory.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "systems.h"

game_t *
game_create(map_t *map, bxr_vec2_t spawn_position)
{
  game_t *game = NULL;
  BXR_NEW(game);
  if (game == NULL) {
    BXR_LOG_ERROR("Failed to allocate memory for game");
    return NULL;
  }

  game->ecs = bxr_ecs_create(GAME_MAX_ENTITIES);
  if (!game->ecs) {
    BXR_LOG_ERROR("Failed to create ECS");
    game_destroy(game);
    return NULL;
  }

  game->map = map;

  game->camera = camera_create(
      (bxr_rect_t){ 0, 0, WINDOW_VIRTUAL_WIDTH, WINDOW_VIRTUAL_HEIGHT },
      map_get_bounds(map));
  if (!game->camera) {
    BXR_LOG_ERROR("Failed to create camera");
    bxr_ecs_destroy(game->ecs);
    game_destroy(game);
    return NULL;
  }

  game->player_animation
      = animation_create((animation_desc_t *)&PLAYER_ANIMATION_DESC);
  if (!game->player_animation) {
    BXR_LOG_ERROR("Failed to create player animation");
    bxr_ecs_destroy(game->ecs);
    camera_destroy(game->camera);
    game_destroy(game);
    return NULL;
  }

  components_register(game);
  systems_register(game);

  game->player_entity = factory_player_create(
      game->ecs, spawn_position, game->player_animation);

  return game;
}

void
game_update(game_t *game, double delta_ms)
{
  game->delta_ms = delta_ms;

  bxr_ecs_system_run(game->ecs, system_player_update, 0);
  bxr_ecs_system_run(game->ecs, system_animation_update, 0);
}

void
game_render(game_t *game, double alpha_ms)
{
  game->alpha_ms = alpha_ms;

  bxr_vec2_t *player_position = bxr_ecs_entity_get_component(
      game->ecs, game->player_entity, component_position);

  bxr_vec2_t target = {
    .x = bxr_floorf(player_position->x),
    .y = bxr_floorf(player_position->y),
  };

  camera_track(game->camera, target);

  bxr_ecs_system_run(game->ecs, system_map_render, 0);
  bxr_ecs_system_run(game->ecs, system_animation_render, 0);
}

void
game_destroy(game_t *game)
{
  if (!game) {
    return;
  }

  animation_destroy(game->player_animation);

  camera_destroy(game->camera);

  bxr_ecs_destroy(game->ecs);

  BXR_FREE(game);
}
