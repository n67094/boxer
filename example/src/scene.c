#include "scene.h"

static void
empty()
{
}

scene_t scene
    = { .setup = empty, .update = empty, .render = empty, .teardown = empty };

void
scene_setup()
{
  scene.setup();
}
void
scene_update(double delta_ms)
{
  scene.update(delta_ms);
}
void
scene_render(double alpha_ms)
{
  scene.render(alpha_ms);
}
void
scene_teardown()
{
  scene.teardown();
}

void
scene_go_to(scene_t next)
{
  scene.teardown();
  scene = next;
  scene.setup();
}
