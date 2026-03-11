#include "example_blend.h"

#include "engine.h"

static void
example_blend_draw_rects(int brightness, int alpha)
{

  // Red rectangle
  engine_painter_set_color(engine_color_make(255, 0, 0, 128));

  // Green rectangle
  engine_painter_set_color(engine_color_make(255, 0, 255, 128));

  // Blue rectangle
  engine_painter_set_color(engine_color_make(255, 0, 255, 128));
}

void
example_blend_setup(void)
{
}

void
example_blend_update(Uint64 delta_time_ms)
{
}

void
example_blend_render(Uint64 alpha_time_ms)
{
}

void
example_blend_shutdown(void)
{
}
