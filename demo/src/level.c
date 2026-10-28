#include "level.h"
#include "defs.h"
#include "game.h"
#include "map.h"

static map_t *_map   = NULL;
static game_t *_game = NULL;

void
level_setup(void)
{
  _map = map_create(ASSET_LEVEL_TILESET,
                    LEVEL_MAP,
                    LEVEL_TILE_SIZE,
                    LEVEL_MAP_WIDTH,
                    LEVEL_MAP_HEIGHT);

  if (!_map) {
    BXR_LOG_ERROR("Failed to load map");
    return;
  }

  _game = game_create(_map, LEVEL_SPAWN_POSITION);
  if (!_game) {
    BXR_LOG_ERROR("Failed to create game");
    return;
  }
}

void
level_update(double delta_ms)
{
  game_update(_game, delta_ms);
}

void
level_render(double alpha_ms)
{
  game_render(_game, alpha_ms);
}

void
level_teardown(void)
{
  if (_game) {
    game_destroy(_game);
    _game = NULL;
  }

  if (_map) {
    map_destroy(_map);
    _map = NULL;
  }
}
