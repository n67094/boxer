#include "engine.h"

#include "test_font.h"

void
test_font_setup(void)
{
  //
}

void
test_font_update(Uint64 delta_time_ms)
{
  //
}

void
test_font_render(Uint64 alpha_time_ms)
{
  engine_painter_set_color(0, 0, 0, 255);
  engine_painter_clear();
}

void
test_font_shutdown(void)
{
  //
}
