/**
 * @file engine_font.h
 *
 * @brief Font utilities.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_FONT_H_
#define ENGINE_FONT_H_

#include <SDL3/SDL.h>

#include "engine_image.h"
#include "engine_math.h"

typedef struct engine_font_s engine_font_t;

engine_font_t *engine_font_ttf_load(const char *path, unsigned int size);

engine_font_t *
engine_font_ttf_mem(const void *data, size_t len, unsigned int size);

engine_font_t *engine_font_atlas_load(const char *path,
                                      engine_rect_t *glyphs,
                                      size_t glyph_count,
                                      int spacing,
                                      int line_spacing);

engine_font_t *engine_font_atlas_mem(unsigned int width,
                                     unsigned int height,
                                     const void *data,
                                     engine_rect_t *glyphs,
                                     size_t glyph_count,
                                     int spacing,
                                     int line_spacing);

void engine_font_destroy(engine_font_t *font);

int engine_font_get_height(const engine_font_t *font);

int engine_font_get_spacing(const engine_font_t *font);
void engine_font_set_spacing(engine_font_t *font, int spacing);

int engine_font_get_line_spacing(const engine_font_t *font);
void engine_font_set_line_spacing(engine_font_t *font, int line_spacing);

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
engine_vec2_t engine_font_measure(const engine_font_t *font, const char *text);

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
char *engine_font_wrap(const engine_font_t *font,
                       const char *text,
                       float max_width,
                       float *height,
                       int tab_size,
                       engine_font_align_e align);

engine_image_t engine_font_render(const engine_font_t *font, const char *text);

#endif // ENGINE_FONT_H_
