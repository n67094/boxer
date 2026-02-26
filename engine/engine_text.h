/**
 * @file engine_text.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_TEXT_H_
#define ENGINE_TEXT_H_

#include <SDL3/SDL.h>

#include "engine_font.h"
#include "engine_math.h"

typedef enum
{
  ENGINE_TEXT_ALIGN_LEFT,
  ENGINE_TEXT_ALIGN_CENTER,
  ENGINE_TEXT_ALIGN_RIGHT,
} engine_text_align_e;

typedef struct engine_text_s engine_text_t;

/**
 * @brief Measure the width and height of the given text when rendered with the
 * specified font.
 *
 * @details Tag color are not counted in the measurement, here is the syntax for
 * tag color: [c=#RRGGBBAA]text[/c]
 *
 * @param font The font to use for measuring the text.
 * @param text The text string to measure.
 * @return An engine_vec2_t containing the width (x) and height (y) of the
 *         measured text.
 */
engine_vec2_t engine_text_measure(const engine_font_t *font, const char *text);

/** @brief Wrap the given text to fit within the specified maximum width when
 * rendered with the provided font.
 *
 * @details This function will insert newline characters into the text to ensure
 * that it does not exceed the specified maximum width. Tag color are not
 * counted in the measurement, here is the syntax for tag color:
 * [c=#RRGGBBAA]text[/c]
 *
 * @param font The font to use for measuring and wrapping the text.
 * @param text The text string to wrap.
 * @param max_width The maximum width (in pixels) that the wrapped text should
 *                  fit within.
 * @param height An optional output parameter that will be set to the total
 * height.
 * @param tab_size The number of spaces to replace tab characters with.
 * @param align The alignment of the text (left, center, right).
 *
 * @return A newly allocated string containing the wrapped text. The caller is
 *         responsible for freeing this string using `engine_mem_free()`.
 */
char *engine_text_wrap(const engine_font_t *font,
                       const char *text,
                       float max_width,
                       float *height,
                       int tab_size,
                       engine_text_align_e align);

// TODO Painter

/**
 * @brief Draw text using the provided font and color at the specified position.
 *
 * @details Multi color text is possible using tag color here is the syntax:
 * [c=#RRGGBBAA]text[/c]
 *
 * @param font The font to use for rendering the text.
 * @param text The text string to render.
 * @param x The x-coordinate of the text's starting position.
 * @param y The y-coordinate of the text's baseline position.
 * @param color The color to render the text with.
 */
/*
engine_text_cache_t *engine_painter_draw_text(const engine_font_t *font,
                                              const char *text,
                                              float x,
                                              float y,
                                              engine_color_t color);
*/

#endif // ENGINE_TEXT_H_
