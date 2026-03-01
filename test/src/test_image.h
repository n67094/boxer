/**
 * @file test_image.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_IMAGE_H_
#define TEST_IMAGE_H_

#include <SDL3/SDL.h>

void test_image_setup(void);

void test_image_update(Uint64 delta_time_ms);

void test_image_render(Uint64 alpha_time_ms);

void test_image_shutdown(void);

#endif // TEST_IMAGE_H_
