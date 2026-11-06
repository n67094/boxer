/**
 * File `bxr_gamepad_internal.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Gamepad (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_GAMEPAD_INTERNAL_H_
#define BXR_GAMEPAD_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_gamepad.h"

bool bxr_gamepad_setup(void);
void bxr_gamepad_teardown(void);
void bxr_gamepad_listen(void);
void bxr_gamepad_begin_frame(void);
void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button);
void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button);
void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, Sint16 value);

#endif // BXR_GAMEPAD_INTERNAL_H_
