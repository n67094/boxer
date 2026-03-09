#include "engine_draw.h"
#include "engine_painter.h"

void
engine_draw_line(engine_line_t line, engine_color_t color)
{
  engine_draw_line_ex(line, color, 1.0f);
}

void
engine_draw_line_ex(engine_line_t line, engine_color_t color, float thickness)
{
  engine_color_t prev_color = engine_painter_get_color();
  engine_painter_set_color(color);

  engine_vec2_t start = line.p1;
  engine_vec2_t end   = line.p2;

  float width = end.x - start.x;

  engine_painter_draw_rect_filled(
      engine_rect_make(start.x, start.y, width, thickness));

  engine_painter_set_color(prev_color);
}

void
engine_draw_rect(engine_rect_t rect, engine_color_t color)
{
  engine_draw_rect_ex(rect, color, 1.0f);
}

void
engine_draw_rect_ex(engine_rect_t rect, engine_color_t color, float thickness)
{
  engine_color_t prev_color = engine_painter_get_color();
  engine_painter_set_color(color);

  // Top
  engine_painter_draw_rect_filled(
      engine_rect_make(rect.x, rect.y, rect.w, thickness));
  // Bottom
  engine_painter_draw_rect_filled(
      engine_rect_make(rect.x, rect.y + rect.h - thickness, rect.w, thickness));
  // Left
  engine_painter_draw_rect_filled(
      engine_rect_make(rect.x, rect.y, thickness, rect.h));
  // Right
  engine_painter_draw_rect_filled(
      engine_rect_make(rect.x + rect.w - thickness, rect.y, thickness, rect.h));

  engine_painter_set_color(prev_color);
}

void
engine_draw_rect_filled(engine_rect_t rect, engine_color_t color)
{
  engine_color_t prev_color = engine_painter_get_color();
  engine_painter_set_color(color);

  engine_painter_draw_rect_filled(rect);

  engine_painter_set_color(prev_color);
}

void
engine_draw_text(engine_vec2_t position,
                 const char *text_str,
                 const engine_font_t *font,
                 engine_color_t background,
                 engine_color_t foreground)
{
  engine_text_t *text
      = engine_text_make(position, font, text_str, background, foreground);
  if (text) {
    engine_draw_text_cached(position, text);
    engine_text_destroy(text);
  }
}

void
engine_draw_text_cached(engine_vec2_t position, const engine_text_t *text)
{
  SDL_assert(text);

  const engine_text_entry_t *entries = engine_text_get_text_entries(text);
  size_t entries_count               = engine_text_get_text_entries_count(text);

  for (size_t i = 0; i < entries_count; ++i) {
    engine_painter_reset_image();

    engine_painter_set_color(entries[i].background);
    engine_painter_draw_rect_filled(entries[i].rects.dst);

    engine_painter_set_image(engine_font_get_image(text->font));

    engine_painter_set_color(entries[i].foreground);
    engine_painter_draw_rect_textured(
        engine_textured_rect_make(entries[i].rects.dst, entries[i].rects.src));
  }
}

void
engine_draw_image(engine_vec2_t position,
                  engine_image_t image,
                  engine_color_t color,
                  float rotation,
                  engine_vec2_t scale,
                  engine_vec2_t origin)
{
  engine_image_t prev_image = engine_painter_get_image();
  engine_painter_set_image(image);
  engine_color_t prev_color = engine_painter_get_color();
  engine_painter_set_color(color);

  int width  = engine_image_get_width(image);
  int height = engine_image_get_height(image);

  engine_rect_t src = engine_rect_make(0, 0, width, height);
  engine_rect_t dst = engine_rect_make(
      position.x, position.y, width * scale.x, height * scale.y);

  engine_painter_push_transform();

  if (rotation != 0.0f) {
    engine_painter_rotate_at(
        rotation, position.x + origin.x, position.y + origin.y);
  }

  engine_painter_draw_rect_textured(engine_textured_rect_make(dst, src));

  engine_painter_pop_transform();
  engine_painter_set_color(prev_color);
  engine_painter_set_image(prev_image);
}
