/**
 * @file engine_color.h
 *
 * @brief Color utilities.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_COLOR_H_
#define ENGINE_COLOR_H_

#include <SDL3/SDL.h>

typedef struct engine_color_s
{
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
} engine_color_t;

#define ENGINE_COLOR_WHITE                                                     \
  (engine_color_t){ .r = 255, .g = 255, .b = 255, .a = 255 }
#define ENGINE_COLOR_BLACK (engine_color_t){ .r = 0, .g = 0, .b = 0, .a = 255 }
#define ENGINE_COLOR_RED (engine_color_t){ .r = 255, .g = 0, .b = 0, .a = 255 }
#define ENGINE_COLOR_GREEN                                                     \
  (engine_color_t){ .r = 0, .g = 255, .b = 0, .a = 255 }
#define ENGINE_COLOR_BLUE (engine_color_t){ .r = 0, .g = 0, .b = 255, .a = 255 }
#define ENGIEN_COLOR_PINK                                                      \
  (engine_color_t){ .r = 255, .g = 0, .b = 255, .a = 255 }
#define ENGINE_COLOR_TRANSPARENT                                               \
  (engine_color_t){ .r = 0, .g = 0, .b = 0, .a = 0 }

#define engine_color_make(r, g, b, a) ((const engine_color_t){ r, g, b, a })

#endif // ENGINE_COLOR_H_
