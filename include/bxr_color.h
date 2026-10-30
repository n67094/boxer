/**
 * File `bxr_color.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Color Utilities
 */

#ifndef BXR_COLOR_H_
#define BXR_COLOR_H_

#include <SDL3/SDL.h>

/**
 * RGBA color with 8 bits per channel, from 0 to 255.
 *
 * `a` is the opacity: 0 is fully transparent, 255 is fully opaque.
 */
typedef struct bxr_color_s
{
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
} bxr_color_t;

/**
 * Predefined opaque colors, plus `BXR_COLOR_TRANSPARENT`.
 */
#define BXR_COLOR_WHITE (bxr_color_t){ .r = 255, .g = 255, .b = 255, .a = 255 }
#define BXR_COLOR_BLACK (bxr_color_t){ .r = 0, .g = 0, .b = 0, .a = 255 }
#define BXR_COLOR_RED (bxr_color_t){ .r = 255, .g = 0, .b = 0, .a = 255 }
#define BXR_COLOR_GREEN (bxr_color_t){ .r = 0, .g = 255, .b = 0, .a = 255 }
#define BXR_COLOR_BLUE (bxr_color_t){ .r = 0, .g = 0, .b = 255, .a = 255 }
#define BXR_COLOR_MAGENTA (bxr_color_t){ .r = 255, .g = 0, .b = 255, .a = 255 }
#define BXR_COLOR_CYAN (bxr_color_t){ .r = 0, .g = 255, .b = 255, .a = 255 }
#define BXR_COLOR_YELLOW (bxr_color_t){ .r = 255, .g = 255, .b = 0, .a = 255 }
#define BXR_COLOR_TRANSPARENT (bxr_color_t){ .r = 0, .g = 0, .b = 0, .a = 0 }

/**
 * Build a color from its channels.
 *
 * `r` red channel, from 0 to 255.
 * `g` green channel, from 0 to 255.
 * `b` blue channel, from 0 to 255.
 * `a` alpha channel, from 0 (transparent) to 255 (opaque).
 *
 * `return` a `bxr_color_t` compound literal, by value.
 */
#define bxr_color_create(r, g, b, a) ((const bxr_color_t){ r, g, b, a })

#endif // BXR_COLOR_H_
