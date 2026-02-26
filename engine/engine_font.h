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

#endif // ENGINE_FONT_H_
