/**
 * @file example_image.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef EXAMPLE_IMAGE_H_
#define EXAMPLE_IMAGE_H_

#include <SDL3/SDL.h>

void example_image_setup(void);

void example_image_update(Uint64 delta_time_ms);

void example_image_render(Uint64 alpha_time_ms);

void example_image_shutdown(void);

#endif // EXAMPLE_IMAGE_H_
