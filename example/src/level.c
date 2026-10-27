#include "level.h"
#include "defs.h"
#include "game.h"
#include "map.h"

static map_t *_map = NULL;

void
level_setup(void)
{
  // Setup the menu and level systems at once for simplicity.
  game_setup();

  _map
      = map_create(ASSET_LEVEL_TILESET, LEVEL_TILES, LEVEL_WIDTH, LEVEL_HEIGHT);

  if (!_map) {
    BXR_LOG_ERROR("Failed to load map");
    return;
  }

  game_set_map(_map);
}

void
level_update(double delta_ms)
{
  game_update(delta_ms);
}

void
level_render(double alpha_ms)
{
  game_render(alpha_ms);
}

void
level_teardown(void)
{
  if (_map) {
    map_destroy(_map);
    _map = NULL;
  }

  game_teardown();
}
