/**
 * @file engine_font.h
 *
 * @brief Font utilities.
 *
 * @details ASCII characters are supported.
 *
 * The font atlas is arranged at your will, glyphs are defined by an array of
 * rectangles that specify the position and size of each character in the atlas.
 *
 * The array should start with your icons (if you have any) and then continue
 * with the characters in the order of ASCII characters from 32 (space) to 255.
 *
 * To render:
 *
 * - An icons use the syntax {i=icon_index}.
 * - Colored text use the syntax {c=#RRGGBBAA}text{/c}.
 *
 * (You can use rxi atlas generator to generate the atlas and the corresponding
 * glyphs array: https://github.com/rxi/atlas)
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_FONT_H_
#define ENGINE_FONT_H_

#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_math.h"

typedef struct engine_font_s engine_font_t;

engine_font_t *engine_font_atlas_load(const char *path,
                                      engine_rect_t *glyphs,
                                      size_t glyph_count,
                                      int glyph_spacing_index,
                                      int char_spacing,
                                      int line_spacing);

engine_font_t *engine_font_atlas_mem(unsigned int width,
                                     unsigned int height,
                                     const void *data,
                                     engine_rect_t *glyphs,
                                     size_t glyph_count,
                                     int glyph_spacing_index,
                                     int char_spacing,
                                     int line_spacing);

void engine_font_destroy(engine_font_t *font);

int engine_font_get_char_spacing(const engine_font_t *font);
void engine_font_set_char_spacing(engine_font_t *font, int spacing);

int engine_font_get_line_spacing(const engine_font_t *font);
void engine_font_set_line_spacing(engine_font_t *font, int line_spacing);

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

// FIXME the function below should should return engine_textured_rect_t to use
// with the painter system.

void engine_font_render_glyph(engine_vec2_t position,
                              const engine_font_t *font,
                              int glyphs_index,
                              engine_color_t background,
                              engine_color_t foreground);

void engine_font_render_char(engine_vec2_t position,
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
void engine_font_render_text(engine_vec2_t position,
                             const engine_font_t *font,
                             const char *text,
                             engine_color_t background,
                             engine_color_t foreground);

#endif // ENGINE_FONT_H_
