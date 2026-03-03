/**
 * @file engine_font.h
 *
 * @brief Font utilities.
 *
 * @details ASCII characters are supported.
 *
 * If you load the font from a TTF file, the engine will generate a font atlas
 * for you.
 *
 * The font atlas is arranged at your will, glyphs are defined by an array of
 * rectangles that specify the position and size of each character in the atlas.
 * The array should start with your icons (if you have any) and then continue
 * with the characters in the order of ASCII characters from 32 (space) to 255.
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

typedef enum
{
  ENGINE_FONT_TYPE_TTF,
  ENGINE_FONT_TYPE_ATLAS,
} engine_font_type_e;

typedef struct engine_font_ttf_s
{
  engine_image_t image;
  engine_rect_t *glyphs;
  size_t glyph_count;
  engine_vec2_t icon_range;
  engine_vec2_t char_range;
  int char_spacing;
  int line_spacing;

} engine_font_ttf_t;

typedef struct engine_font_atlas_s
{
  engine_image_t image;
  const engine_rect_t *glyphs;
  size_t glyph_count;
  engine_vec2_t icon_range;
  engine_vec2_t char_range;
  int char_spacing;
  int line_spacing;
} engine_font_atlas_t;

typedef struct engine_font_s
{
  engine_font_type_e type;
  union
  {
    engine_font_ttf_t ttf;
    engine_font_atlas_t atlas;
  };
} engine_font_t;

engine_font_t *engine_font_load_ttf(const char *path,
                                    engine_vec2_t icon_range,
                                    engine_vec2_t char_range,
                                    int font_size,
                                    int char_spacing,
                                    int line_spacing);

engine_font_t *engine_font_load_ttf_mem(const void *data,
                                        size_t data_size,
                                        engine_vec2_t icon_range,
                                        engine_vec2_t char_range,
                                        int font_size,
                                        int char_spacing,
                                        int line_spacing);

engine_font_t *engine_font_atlas_load(const char *path,
                                      const engine_rect_t *glyphs,
                                      size_t glyph_count,
                                      engine_vec2_t icon_range,
                                      engine_vec2_t char_range,
                                      int char_spacing,
                                      int line_spacing);

engine_font_t *engine_font_atlas_mem(unsigned int width,
                                     unsigned int height,
                                     const void *data,
                                     const engine_rect_t *glyphs,
                                     size_t glyph_count,
                                     engine_vec2_t icon_range,
                                     engine_vec2_t char_range,
                                     int char_spacing,
                                     int line_spacing);

void engine_font_destroy(engine_font_t *font);

int engine_font_get_char_spacing(const engine_font_t *font);
void engine_font_set_char_spacing(engine_font_t *font, int spacing);

int engine_font_get_line_spacing(const engine_font_t *font);
void engine_font_set_line_spacing(engine_font_t *font, int line_spacing);

engine_vec2_t engine_font_get_icon_range(const engine_font_t *font);
engine_vec2_t engine_font_get_char_range(const engine_font_t *font);

int engine_font_get_glyph_count(const engine_font_t *font);

engine_rect_t engine_font_get_glyph_rect(const engine_font_t *font,
                                         int glyphs_index);

#endif // ENGINE_FONT_H_
