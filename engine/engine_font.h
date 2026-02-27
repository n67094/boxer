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
 * The mapping from characters to glyphs is determined by the order of the
 * rectangles in the array, starting from the ASCII character 32 to
 * 255 (up to 224 glyphs) where 32 is the space character, 33 is the exclamation
 * mark, and so on.
 *
 * If you need icons you can add them to the end of the glyphs array (past the
 * 224th character which is nbsp) and use them with this tag: {i=glyph_index}
 * or you can fill the empty space in the atlas with icons and use them as
 * regular characters.
 *
 * The font rendering functions support color tags, allowing you to specify
 * different colors for different parts of the text example:
 * {c=#RRGGBBAA}text{/c}.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_FONT_H_
#define ENGINE_FONT_H_

#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_image.h"
#include "engine_math.h"

typedef struct engine_font_s engine_font_t;

engine_font_t *engine_font_atlas_load(const char *path,
                                      engine_rect_t *glyphs,
                                      size_t glyph_count,
                                      int tab_size,
                                      int char_spacing,
                                      int line_spacing);

engine_font_t *engine_font_atlas_mem(unsigned int width,
                                     unsigned int height,
                                     const void *data,
                                     engine_rect_t *glyphs,
                                     size_t glyph_count,
                                     int tab_size,
                                     int char_spacing,
                                     int line_spacing);

void engine_font_destroy(engine_font_t *font);

int engine_font_get_char_spacing(const engine_font_t *font);
void engine_font_set_char_spacing(engine_font_t *font, int spacing);

int engine_font_get_line_spacing(const engine_font_t *font);
void engine_font_set_line_spacing(engine_font_t *font, int line_spacing);

/**
 * @brief Measure the width and height of the given text when rendered with the
 * specified font.
 *
 * @param font The font to use for measuring the text.
 * @param text The text string to measure.
 *
 * @return An engine_vec2_t containing the width (x) and height (y) of the
 *         measured text.
 *
 * @note Tags are not counted in the measurement.
 */
engine_vec2_t engine_font_measure_text(const engine_font_t *font,
                                       const char *text);

/**
 * @brief Wrap the given text to fit within the specified maximum width when
 * rendered with the provided font.
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
 * @note Tags are not counted in the measurement.
 * @note The caller is responsible for freeing the returned value.
 */
char *engine_font_wrap_text(const engine_font_t *font,
                            const char *text,
                            float max_width,
                            float *height,
                            engine_font_align_e align);

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
void engine_font_render(engine_vec2_t position,
                        const engine_font_t *font,
                        const char *text,
                        engine_color_t background,
                        engine_color_t foreground);

/**
 * @brief Render the given text at the specified position using the provided
 * font and colors, with word wrapping to fit within the specified maximum
 * width.
 *
 * @param position The position (in pixels) where the text should be rendered.
 * @param font The font to use for rendering the text.
 * @param text The text string to render.
 * @param max_width The maximum width (in pixels) that the rendered text should
 *                  fit within.
 * @param tab_size The number of spaces to replace tab characters with.
 * @param align The alignment of the text (left, center, right).
 * @param background The background color to use when rendering the text.
 * @param foreground The foreground color to use when rendering the text.
 *
 * @note You need to be in between `engine_painter_begin()` and
 * `engine_painter_end()` to call this function.
 */
void engine_font_render_wrapped(engine_vec2_t position,
                                const engine_font_t *font,
                                const char *text,
                                float max_width,
                                engine_font_align_e align,
                                engine_color_t background,
                                engine_color_t foreground);

#endif // ENGINE_FONT_H_
