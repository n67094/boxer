/**
 * @file engine_text.h
 *
 * @brief Text utilities.
 *
 * @details To render:
 *
 * - An icons{i=FF}.
 * - Change Background color  {b=RRGGBBAA}text{/b}.
 * - Change Foreground color {f=RRGGBBAA}text{/f}.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_TEXT_H_
#define ENGINE_TEXT_H_

#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_font.h"
#include "engine_math.h"

typedef struct engine_text_entry_s
{
  engine_textured_rect_t rects;
  engine_color_t background;
  engine_color_t foreground;
} engine_text_entry_t;

typedef struct engine_text_s
{
  engine_text_entry_t *entries;
  size_t entries_count;
  const engine_font_t *font;
} engine_text_t;

typedef enum
{
  ENGINE_TEXT_ALIGN_LEFT = 0,
  ENGINE_TEXT_ALIGN_CENTER,
  ENGINE_TEXT_ALIGN_RIGHT,
  ENGINE_TEXT_ALIGN_SIZE,
} engine_text_align_e;

engine_text_t *_engine_text_glyph_make(const engine_font_t *font,
                                       int glyphs_index,
                                       engine_color_t background,
                                       engine_color_t foreground);

engine_text_t *_engine_text_char_make(const engine_font_t *font,
                                      char c,
                                      engine_color_t background,
                                      engine_color_t foreground);

engine_text_t *engine_text_make(const engine_font_t *font,
                                const char *rich_str,
                                engine_color_t background,
                                engine_color_t foreground);

engine_text_t *engine_text_make_ex(const engine_font_t *font,
                                   const char *rich_str,
                                   engine_color_t background,
                                   engine_color_t foreground,
                                   int max_width,
                                   engine_text_align_e align,
                                   bool ignore_newlines);

void engine_text_destroy(engine_text_t *text);

const engine_text_entry_t *engine_text_get_entries(const engine_text_t *text);

size_t engine_text_get_entries_count(const engine_text_t *text);

// char *engine_text_without_tags(const char *rich_str);

/**
 * @brief Get the length of a text string without counting any formatting tags.
 */
size_t engine_text_length(const char *rich_str);

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
engine_vec2_t engine_text_measure(const engine_font_t *font,
                                  const char *rich_str);

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
char *engine_text_wrap(const engine_font_t *font,
                       const char *rich_str,
                       float max_width,
                       float *height);

#endif // ENGINE_TEXT_H_
