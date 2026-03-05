/**
 * @file example_text.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef EXAMPLE_TEXT_H_
#define EXAMPLE_TEXT_H_

#include <SDL3/SDL.h>

#include "engine.h"

void example_text_setup(void);

void example_text_update(Uint64 delta_time_ms);

void example_text_render(Uint64 alpha_time_ms);

void example_text_shutdown(void);

#endif // EXAMPLE_TEXT_H_
