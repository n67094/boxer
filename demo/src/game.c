#include <boxer/bxr.h>

#include "components.h"
#include "defs.h"
#include "game.h"
#include "map.h"
#include "systems.h"

static Uint32 _initialized = 0;
static game_t _game        = { 0 };

void
game_setup(void)
{
  _initialized = 1;

  _game.ecs = bxr_ecs_create(GAME_MAX_ENTITIES);
  if (!_game.ecs) {
    BXR_LOG_ERROR("Failed to create ECS");
    return;
  }

  components_register(&_game);
  systems_register(&_game);
}

void
game_set_map(map_t *map)
{
  BXR_ASSERT(_initialized && "game must be initialized before setting the map");

  _game.map = map;
}

void
game_update(double delta_ms)
{
  BXR_ASSERT(_initialized && "game must be initialized before updating");

  _game.delta_ms = delta_ms;

  bxr_ecs_system_run(_game.ecs, system_player_update, 0);
  bxr_ecs_system_run(_game.ecs, system_animation_update, 0);
}

void
game_render(double alpha_ms)
{
  BXR_ASSERT(_initialized && "game must be initialized before rendering");

  _game.alpha_ms = alpha_ms;

  bxr_ecs_system_run(_game.ecs, system_map_render, 0);
  bxr_ecs_system_run(_game.ecs, system_animation_render, 0);
}

void
game_teardown(void)
{
  _initialized = 1;

  bxr_ecs_destroy(_game.ecs);
}
