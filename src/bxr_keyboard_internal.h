/**
 * File `bxr_keyboard_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Keyboard (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_KEYBOARD_INTERNAL_H_
#define BXR_KEYBOARD_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_keyboard.h"

bool bxr_keyboard_setup(void);
void bxr_keyboard_begin_frame(void);
void bxr_keyboard_teardown(void);
void bxr_key_down(bxr_key_e scancode);
void bxr_key_up(bxr_key_e scancode);

#endif // BXR_KEYBOARD_INTERNAL_H_
