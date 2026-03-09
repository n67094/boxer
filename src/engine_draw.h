/**
 * @file engine_draw.h
 *
 * @brief High-level drawing utilities.
 *
 * @details These functions are built on top of the low-level painter API (see
 * `engine_painter.h`) and are meant to be used for convenience.
 *
 * Those function must be called between `engine_painter_begin()` and
 * `engine_painter_end()`, and will use the current painter state (color, blend
 * mode, etc.) for drawing.
 *
 * For more control and performance, you can use the painter API directly.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_DRAW_H_
#define ENGINE_DRAW_H_

#include "engine_color.h"
#include "engine_font.h"
#include "engine_geometry.h"
#include "engine_math.h"
#include "engine_text.h"

// Line
// --------------------------------------------------------------------------

void engine_draw_line(engine_line_t line, engine_color_t color);
void
engine_draw_line_ex(engine_line_t line, engine_color_t color, float thickness);

// Rectangle
// --------------------------------------------------------------------------

void engine_draw_rect(engine_rect_t rect, engine_color_t color);
void
engine_draw_rect_ex(engine_rect_t rect, engine_color_t color, float thickness);

void engine_draw_rect_filled(engine_rect_t rect, engine_color_t color);

// Text
// --------------------------------------------------------------------------

void engine_draw_text(engine_vec2_t position,
                      const char *rich_str,
                      const engine_font_t *font,
                      engine_color_t background,
                      engine_color_t foreground);

void engine_draw_text_cached(engine_vec2_t position, const engine_text_t *text);

// Image
// --------------------------------------------------------------------------

void engine_draw_image(engine_vec2_t position,
                       engine_image_t image,
                       engine_color_t color,
                       float rotation,
                       engine_vec2_t scale,
                       engine_vec2_t origin);

#endif // ENGINE_DRAW_H_
