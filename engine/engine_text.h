/**
 * @file engine_text.h
 *
 * @brief Text utilities.
 *
 * @details To render:
 *
 * - An icons{i=icon_index}.
 * - Change Background color  {b=#RRGGBBAA}text{/b}.
 * - Change Foreground color {f=#RRGGBBAA}text{/f}.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_TEXT_H_
#define ENGINE_TEXT_H_

#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_font.h"

typedef struct engine_text_entry_s
{
  engine_rect_t dest_rect;
  engine_rect_t *src_rect;
  engine_color_t background;
  engine_color_t foreground;
} engine_text_entry_t;

typedef struct engine_text_s
{
  engine_text_entry_t *entries;
  size_t entry_count;
} engine_text_t;

// Public API
// -----------------------------------------------------------------------------

engine_text_t *engine_text_glyph(engine_vec2_t position,
                                 const engine_font_t *font,
                                 int glyphs_index,
                                 engine_color_t background,
                                 engine_color_t foreground);

engine_text_t *engine_text_char(engine_vec2_t position,
                                const engine_font_t *font,
                                char c,
                                engine_color_t background,
                                engine_color_t foreground);

/**
 * @brief Render the given text at the specified position using the provided
 * font and colors.
 *
 * @param position The position (in pixels) where the text should be rendered.
 * @param font The font to use for rendering the text.
 * @param text The text string to render.
 * @param background The background color to use when rendering the text.
 * @param foreground The foreground color to use when rendering the text.
 *
 * @note You need to be in between `engine_painter_begin()` and
 * `engine_painter_end()` to call this function.
 */
engine_text_t *engine_text(engine_vec2_t position,
                           const engine_font_t *font,
                           const char *text,
                           engine_color_t background,
                           engine_color_t foreground);

void engine_text_destroy(engine_text_t *text);

// FIXME those function should return engine_text_t instead of rendering
// directly, then the user can use  the painter to render text.

size_t engine_text_length(const char *text);

/**
 * @brief Measure the width and height of the given text when rendered with the
 * specified font. (tags are ignored for measurement)
 *
 * @param font The font to use for measuring the text.
 * @param text The text string to measure.
 *
 * @return An engine_vec2_t containing the width (x) and height (y) of the
 *         measured text.
 */
engine_vec2_t engine_font_measure_text(const engine_font_t *font,
                                       const char *text);

/**
 * @brief Wrap the given text to fit within the specified maximum width when
 * rendered with the provided font. (tags are ignored for
 * wrapping)
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
 *
 * @note The caller is responsible for freeing the returned value.
 */
char *engine_font_wrap_text(const engine_font_t *font,
                            const char *text,
                            float max_width,
                            float *height);

#endif // ENGINE_TEXT_H_
