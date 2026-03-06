#include "engine.h"

#include "example_draw.h"

void
example_draw_setup(void)
{
}

void
example_draw_update(Uint64 delta_time_ms)
{
}

void
example_draw_render(Uint64 alpha_time_ms)
{
  engine_vec2_t painter_dimensions = engine_painter_dimensions();

  // Quadrant 1
  {
    engine_rect_t bounds = engine_rect_make(
        0, 0, painter_dimensions.x / 2, painter_dimensions.y / 2);

    engine_draw_rect_filled(bounds, engine_color_make(25, 25, 25, 255));

    engine_draw_line(
        engine_line_make(
            bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h),
        engine_color_make(255, 0, 0, 255));

    engine_draw_line_ex(
        engine_line_make(
            bounds.x + bounds.w, bounds.y, bounds.x, bounds.y + bounds.h),
        engine_color_make(0, 255, 0, 255),
        3.0f);
  }
  // Quadrant 2
  {
    engine_rect_t bounds = engine_rect_make(painter_dimensions.x / 2,
                                            0,
                                            painter_dimensions.x / 2,
                                            painter_dimensions.y / 2);

    engine_rect_t inner_bounds = engine_rect_make(
        bounds.x + 10, bounds.y + 10, bounds.w - 20, bounds.h - 20);

    engine_draw_rect_ex(inner_bounds, engine_color_make(255, 0, 0, 255), 3.0f);
  }
  // Quadrant 3
  {
    engine_rect_t bounds = engine_rect_make(0,
                                            painter_dimensions.y / 2,
                                            painter_dimensions.x / 2,
                                            painter_dimensions.y / 2);
  }
  // Quadrant 4
  {
    engine_rect_t bounds = engine_rect_make(painter_dimensions.x / 2,
                                            painter_dimensions.y / 2,
                                            painter_dimensions.x / 2,
                                            painter_dimensions.y / 2);

    engine_draw_rect_filled(bounds, engine_color_make(50, 50, 50, 255));
  }
}

void
example_draw_shutdown(void)
{
}
