#include <boxer/bxr.h>

#include "camera.h"
#include "defs.h"
#include "state.h"

struct state_s
{
  game_state_e state;

  bxr_ecs_t *ecs;

  camera_t *camera;
};

static state_t _state = { 0 };

void
state_setup(void)
{
  _state.ecs = bxr_ecs_create(GAME_MAX_ENTITIES);

  _state.camera = camera_create((bxr_rect_t){
      .x = 0,
      .y = 0,
      .w = WINDOW_VIRTUAL_WIDTH,
      .h = WINDOW_VIRTUAL_HEIGHT,
  });
}

void
state_teardown(void)
{
  bxr_ecs_destroy(_state.ecs);

  camera_destroy(_state.camera);
}

void
state_set(game_state_e new_state)
{
  _state.state = new_state;
}

game_state_e
state_get(void)
{
  return _state.state;
}
