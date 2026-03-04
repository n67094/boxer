/**
 * @file engine_font.h
 *
 * @brief Font utilities.
 *
 * @details ASCII characters are supported.
 *
 * Font are defined by an array of rectangles that specify the position and
 * size of each chars or icons within an image. Icons and chars are defined by
 * a range of indices. For example, if you have 4 icons and 96 chars, you can
 * set the icon range to (0, 3) and the char range to (32, 126).
 *
 * (You can use rxi atlas generator to generate the atlas and the corresponding
 * glyphs array: https://github.com/rxi/atlas)
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_FONT_H_
#define ENGINE_FONT_H_

#include <SDL3/SDL.h>

#include "engine_image.h"
#include "engine_math.h"

typedef struct engine_font_s engine_font_t;

engine_font_t *engine_font_atlas_load(const char *path,
                                      const engine_rect_t *glyphs,
                                      size_t glyph_count,
                                      engine_vec2_t icon_range,
                                      engine_vec2_t char_range,
                                      char first_char,
                                      int char_spacing,
                                      int line_spacing);

engine_font_t *engine_font_atlas_mem(unsigned int width,
                                     unsigned int height,
                                     const void *data,
                                     const engine_rect_t *glyphs,
                                     size_t glyph_count,
                                     engine_vec2_t icon_range,
                                     engine_vec2_t char_range,
                                     char first_char,
                                     int char_spacing,
                                     int line_spacing);

void engine_font_destroy(engine_font_t *font);

engine_image_t engine_font_get_image(const engine_font_t *font);

engine_rect_t engine_font_get_glyph_rect(const engine_font_t *font,
                                         int glyphs_index);

int engine_font_get_glyph_count(const engine_font_t *font);

engine_vec2_t engine_font_get_icon_range(const engine_font_t *font);
engine_vec2_t engine_font_get_char_range(const engine_font_t *font);

int engine_font_get_base(const engine_font_t *font);

int engine_font_get_char_spacing(const engine_font_t *font);
void engine_font_set_char_spacing(engine_font_t *font, int spacing);

int engine_font_get_line_spacing(const engine_font_t *font);
void engine_font_set_line_spacing(engine_font_t *font, int line_spacing);

#endif // ENGINE_FONT_H_
