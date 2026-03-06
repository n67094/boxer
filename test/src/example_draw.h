/**
 * @file example_draw.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef EXAMPLE_DRAW_H_
#define EXAMPLE_DRAW_H_

#include <SDL3/SDL.h>

void example_draw_setup(void);

void example_draw_update(Uint64 delta_time_ms);

void example_draw_render(Uint64 alpha_time_ms);

void example_draw_shutdown(void);

#endif // EXAMPLE_DRAW_H_
