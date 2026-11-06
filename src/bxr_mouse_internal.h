/**
 * File `bxr_mouse_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Mouse (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_MOUSE_INTERNAL_H_
#define BXR_MOUSE_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_mouse.h"

bool bxr_mouse_setup(void);
void bxr_mouse_begin_frame(void);
void bxr_mouse_teardown(void);
void bxr_mouse_button_down(bxr_mouse_e button, Uint8 click);
void bxr_mouse_button_up(bxr_mouse_e button);
void bxr_mouse_motion(float x, float y);
void bxr_mouse_wheel_motion(float x, float y);

#endif // BXR_MOUSE_INTERNAL_H_
