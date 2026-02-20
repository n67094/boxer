/**
 * @file debug.h
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "engine.h"

void debug_setup(engine_context_t *context);
void debug_event(SDL_Event *event);
void debug_render(engine_context_t *context);
void debug_shutdown();

#endif // DEBUG_H_
