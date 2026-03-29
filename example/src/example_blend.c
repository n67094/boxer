#include "example_blend.h"

#include "bxr.h"

static void
example_blend_draw_rects(int brightness, int alpha)
{
  // Red rectangle
  bxr_painter_set_color(bxr_color_make(brightness, 0, 0, alpha));
  bxr_painter_draw_rect_filled(bxr_rect_make(10, 10, 50, 50));

  bxr_painter_translate(10, 10);

  // Green rectangle
  bxr_painter_set_color(bxr_color_make(0, brightness, 0, alpha));
  bxr_painter_draw_rect_filled(bxr_rect_make(10, 10, 50, 50));

  bxr_painter_translate(10, 10);

  // Blue rectangle
  bxr_painter_set_color(bxr_color_make(0, 0, brightness, alpha));
  bxr_painter_draw_rect_filled(bxr_rect_make(10, 10, 50, 50));
}

static void
example_blend_draw_checkboard(int width, int height)
{
  int size = 20;
  for (int y = 0; y < height; y += size) {
    for (int x = 0; x < width; x += size) {
      bool is_white     = ((x / size) + (y / size)) % 2 == 0;
      bxr_color_t color = is_white ? bxr_color_make(150, 150, 150, 255)
                                   : bxr_color_make(50, 50, 50, 255);
      bxr_painter_set_color(color);
      bxr_painter_draw_rect_filled(bxr_rect_make(x, y, size, size));
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

  bxr_painter_set_color(bxr_color_make(0, 0, 0, 255));
  bxr_painter_clear();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_NONE);

  bxr_vec2_t frame_dimension = bxr_painter_get_frame_dimension();
  example_blend_draw_checkboard(frame_dimension.x, frame_dimension.y);

  bxr_painter_set_blend_mode(BXR_BLENDMODE_NONE);
  bxr_painter_push_transform();
  bxr_painter_translate(0, 0);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_BLEND);
  bxr_painter_push_transform();
  bxr_painter_translate(80, 0);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_BLEND_PREMULTIPLIED);
  bxr_painter_push_transform();
  bxr_painter_translate(160, 0);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_ADD);
  bxr_painter_push_transform();
  bxr_painter_translate(80, 80);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_ADD_PREMULTIPLIED);
  bxr_painter_push_transform();
  bxr_painter_translate(160, 80);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_MOD);
  bxr_painter_push_transform();
  bxr_painter_translate(80, 160);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();

  bxr_painter_set_blend_mode(BXR_BLENDMODE_MUL);
  bxr_painter_push_transform();
  bxr_painter_translate(160, 160);
  example_blend_draw_rects(brightness, alpha);
  bxr_painter_pop_transform();
}

void
example_blend_shutdown(void)
{
}
