/**
 * @file bxr_text.h
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

#ifndef BXR_TEXT_H_
#define BXR_TEXT_H_

#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_font.h"
#include "bxr_math.h"

typedef struct bxr_text_entry_s
{
  bxr_textured_rect_t rects;
  bxr_color_t background;
  bxr_color_t foreground;
} bxr_text_entry_t;

typedef struct bxr_text_s
{
  bxr_text_entry_t *entries;
  size_t entries_count;
  const bxr_font_t *font;
} bxr_text_t;

typedef enum
{
  BXR_TEXT_ALIGN_LEFT = 0,
  BXR_TEXT_ALIGN_CENTER,
  BXR_TEXT_ALIGN_RIGHT,
  BXR_TEXT_ALIGN_SIZE,
} bxr_text_align_e;

bxr_text_t *_bxr_text_glyph_make(const bxr_font_t *font,
                                 int glyphs_index,
                                 bxr_color_t background,
                                 bxr_color_t foreground);

bxr_text_t *_bxr_text_char_make(const bxr_font_t *font,
                                char c,
                                bxr_color_t background,
                                bxr_color_t foreground);

bxr_text_t *bxr_text_make(const bxr_font_t *font,
                          const char *rich_str,
                          bxr_color_t background,
                          bxr_color_t foreground);

bxr_text_t *bxr_text_make_ex(const bxr_font_t *font,
                             const char *rich_str,
                             bxr_color_t background,
                             bxr_color_t foreground,
                             int max_width,
                             bxr_text_align_e align,
                             bool ignore_newlines);

void bxr_text_destroy(bxr_text_t *text);

const bxr_text_entry_t *bxr_text_get_entries(const bxr_text_t *text);

size_t bxr_text_get_entries_count(const bxr_text_t *text);

// char *bxr_text_without_tags(const char *rich_str);

/**
 * @brief Get the length of a text string without counting any formatting tags.
 */
size_t bxr_text_length(const char *rich_str);

/**
 * @brief Measure the width and height of the given text when rendered with the
 * specified font. (tags are ignored for measurement)
 *
 * @param font The font to use for measuring the text.
 * @param text The text string to measure.
 *
 * @return An bxr_vec2_t containing the width (x) and height (y) of the
 *         measured text.
 */
bxr_vec2_t bxr_text_measure(const bxr_font_t *font, const char *rich_str);

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
 *         responsible for freeing this string using `bxr_mem_free()`.
 *
 * @note The caller is responsible for freeing the returned value.
 */
char *bxr_text_wrap(const bxr_font_t *font,
                    const char *rich_str,
                    float max_width,
                    float *height);

#endif // BXR_TEXT_H_
