#include <boxer/bxr.h>

#include "components.h"
#include "defs.h"
#include "factory.h"
#include "game.h"
#include "map.h"
#include "systems.h"

game_t *
game_create(map_t *map)
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
    return NULL;
  }

  game->map = map;

  components_register(game);
  systems_register(game);

  /*
  factory_player_desc_t player_desc = {
    .position = { 32, 32 },
    .collider = { 0, 0, 8, 8 },
  };

  game->player_entity = factory_player_create(game->ecs, &player_desc);
  */

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

  bxr_ecs_system_run(game->ecs, system_map_render, 0);
  bxr_ecs_system_run(game->ecs, system_animation_render, 0);
}

void
game_destroy(game_t *game)
{
  BXR_ASSERT(game);
}
