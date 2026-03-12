#include "example_blend.h"

#include "engine.h"

static void
example_blend_draw_rects(int brightness, int alpha)
{
  // Red rectangle
  engine_painter_set_color(engine_color_make(brightness, 0, 0, alpha));
  engine_painter_draw_rect_filled(engine_rect_make(10, 10, 50, 50));

  engine_painter_translate(10, 10);

  // Green rectangle
  engine_painter_set_color(engine_color_make(0, brightness, 0, alpha));
  engine_painter_draw_rect_filled(engine_rect_make(10, 10, 50, 50));

  engine_painter_translate(10, 10);

  // Blue rectangle
  engine_painter_set_color(engine_color_make(0, 0, brightness, alpha));
  engine_painter_draw_rect_filled(engine_rect_make(10, 10, 50, 50));
}

static void
example_blend_draw_checkboard(int width, int height)
{
  int size = 20;
  for (int y = 0; y < height; y += size) {
    for (int x = 0; x < width; x += size) {
      bool is_white        = ((x / size) + (y / size)) % 2 == 0;
      engine_color_t color = is_white ? engine_color_make(150, 150, 150, 255)
                                      : engine_color_make(50, 50, 50, 255);
      engine_painter_set_color(color);
      engine_painter_draw_rect_filled(engine_rect_make(x, y, size, size));
    }
  }
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
  int brightness = 255;
  int alpha      = 128;

  engine_painter_set_color(engine_color_make(0, 0, 0, 255));
  engine_painter_clear();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_NONE);

  engine_vec2_t frame_size = engine_painter_get_frame_size();
  example_blend_draw_checkboard(frame_size.x, frame_size.y);

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_NONE);
  engine_painter_push_transform();
  engine_painter_translate(0, 0);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_BLEND);
  engine_painter_push_transform();
  engine_painter_translate(80, 0);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_BLEND_PREMULTIPLIED);
  engine_painter_push_transform();
  engine_painter_translate(160, 0);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_ADD);
  engine_painter_push_transform();
  engine_painter_translate(80, 80);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_ADD_PREMULTIPLIED);
  engine_painter_push_transform();
  engine_painter_translate(160, 80);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_MOD);
  engine_painter_push_transform();
  engine_painter_translate(80, 160);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();

  engine_painter_set_blend_mode(ENGINE_BLENDMODE_MUL);
  engine_painter_push_transform();
  engine_painter_translate(160, 160);
  example_blend_draw_rects(brightness, alpha);
  engine_painter_pop_transform();
}

void
example_blend_shutdown(void)
{
}
