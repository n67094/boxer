/**
 * File `bxr_font.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Font Atlas Utilities
 *
 * Font are defined by an array of rectangles that specify the position and
 * size of each chars or icons within an image. Icons and chars are defined by
 * a range of indices. For example, if you have 4 icons and 96 chars, you can
 * set the icon range to (0, 3) and the char range to (32, 126).
 */

// TODO add a default font and docs (font data should be nestead in font.c
// files).

#ifndef BXR_FONT_H_
#define BXR_FONT_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_image.h"
#include "bxr_math.h"

typedef struct bxr_font_s bxr_font_t;

BXR_API bxr_font_t *bxr_font_create(const char *path,
                                    const bxr_rect_t *glyphs,
                                    size_t glyph_count,
                                    bxr_vec2_t icon_range,
                                    bxr_vec2_t char_range,
                                    char first_char,
                                    int char_spacing,
                                    int line_spacing);

BXR_API bxr_font_t *bxr_font_create_mem(unsigned int width,
                                        unsigned int height,
                                        const void *data,
                                        const bxr_rect_t *glyphs,
                                        size_t glyph_count,
                                        bxr_vec2_t icon_range,
                                        bxr_vec2_t char_range,
                                        char first_char,
                                        int char_spacing,
                                        int line_spacing);

BXR_API void bxr_font_destroy(bxr_font_t *font);

BXR_API bxr_image_t bxr_font_get_image(const bxr_font_t *font);

BXR_API bxr_rect_t bxr_font_get_glyph_rect(const bxr_font_t *font,
                                           int glyphs_index);

BXR_API int bxr_font_get_glyph_count(const bxr_font_t *font);

BXR_API bxr_vec2_t bxr_font_get_icon_range(const bxr_font_t *font);
BXR_API bxr_vec2_t bxr_font_get_char_range(const bxr_font_t *font);

BXR_API int bxr_font_get_base(const bxr_font_t *font);

BXR_API int bxr_font_get_char_spacing(const bxr_font_t *font);
BXR_API void bxr_font_set_char_spacing(bxr_font_t *font, int spacing);

BXR_API int bxr_font_get_line_spacing(const bxr_font_t *font);
BXR_API void bxr_font_set_line_spacing(bxr_font_t *font, int line_spacing);

BXR_API bxr_vec2_t bxr_font_measure_text(const bxr_font_t *font,
                                         const char *str);

#endif // BXR_FONT_H_
