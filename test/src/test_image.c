#include <SDL3/SDL.h>

#include "engine.h"

#include "test_image.h"

static engine_image_t _image;

void
test_image_setup(void)
{
  _image = engine_image_load("data/images/logo.png");
}

void
test_image_update(Uint64 delta_time_ms)
{
  //
}

void
test_image_render(Uint64 alpha_time_ms)
{
  engine_painter_set_color(0, 0, 0, 255);
  engine_painter_clear();

  // Seconds since start
  float ticks = (float)SDL_GetTicks() / 1000.0f;

  engine_vec2_t painter_dimensions = engine_painter_dimensions();

  engine_vec2_t center = engine_vec2_scale(painter_dimensions, 0.5f);

  int width  = engine_image_get_width(_image);
  int height = engine_image_get_height(_image);

  engine_rect_t src = engine_rect_make(0, 0, width, height);
  engine_rect_t dst = engine_rect_make(
      center.x - width / 2.0f, center.y - height / 2.0f, width, height);

  engine_painter_set_image(_image);
  engine_painter_set_color(255, 255, 255, 255);

  engine_painter_push_transform();

  engine_painter_rotate_at(-ticks, dst.x + dst.w / 2.0f, dst.y + dst.h / 2.0f);

  engine_painter_draw_textured_rect(dst, src);

  engine_painter_pop_transform();
}

void
test_image_shutdown(void)
{
  engine_image_destroy(_image);
}
