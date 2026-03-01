/**
 * @file test_font.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_FONT_H_
#define TEST_FONT_H_

#include <SDL3/SDL.h>

void test_font_setup(void);

void test_font_update(Uint64 delta_time_ms);

void test_font_render(Uint64 alpha_time_ms);

void test_font_shutdown(void);

#endif // TEST_FONT_H_
