#include <boxer/bxr.h>

#include "camera.h"
#include "defs.h"
#include "game.h"
#include "map.h"

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

  _game.camera = camera_create((bxr_rect_t){
      .x = 0,
      .y = 0,
      .w = WINDOW_VIRTUAL_WIDTH,
      .h = WINDOW_VIRTUAL_HEIGHT,
  });
  if (!_game.camera) {
    BXR_LOG_ERROR("Failed to create camera");
    return;
  }
}

void
game_set_map(map_t *map)
{
  BXR_ASSERT(_initialized && "game must be initialized before setting the map");

  _game.map = map;
}

map_t *
game_get_map(void)
{
  BXR_ASSERT(_initialized && "game must be initialized before getting the map");

  return _game.map;
}

void
game_set_delta(double delta_ms)
{
  BXR_ASSERT(_initialized
             && "game must be initialized before setting delta time");

  _game.delta_ms = delta_ms;
}

float
game_get_delta(void)
{
  BXR_ASSERT(_initialized
             && "game must be initialized before getting delta time");

  return (float)_game.delta_ms;
}

void
game_set_alpha(double alpha_ms)
{
  BXR_ASSERT(_initialized
             && "game must be initialized before setting alpha time");

  _game.alpha_ms = alpha_ms;
}

float
game_get_alpha(void)
{
  BXR_ASSERT(_initialized
             && "game must be initialized before getting alpha time");

  return (float)_game.alpha_ms;
}

void
game_teardown(void)
{
  _initialized = 1;

  camera_destroy(_game.camera);

  bxr_ecs_destroy(_game.ecs);
}
