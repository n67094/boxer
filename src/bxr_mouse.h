/**
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * @brief Mouse input handling.
 *
 * @details This module provides functions for handling mouse input, including
 * mouse position, button states, wheel motion, and cursor visibility.
 */

#ifndef BXR_MOUSE_H_
#define BXR_MOUSE_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"

typedef enum
{
  BXR_MOUSE_NONE = 0,
  BXR_MOUSE_SINGLE,
  BXR_MOUSE_DOUBLE,
} bxr_mouse_click_e;

typedef enum
{
  BXR_MOUSE_UNKNOWN = 0,
  BXR_MOUSE_LEFT    = SDL_BUTTON_LEFT,
  BXR_MOUSE_MIDDLE  = SDL_BUTTON_MIDDLE,
  BXR_MOUSE_RIGHT   = SDL_BUTTON_RIGHT,
  BXR_MOUSE_X1      = SDL_BUTTON_X1,
  BXR_MOUSE_X2      = SDL_BUTTON_X2,
  BXR_MOUSE_SIZE    = 6,
} bxr_mouse_e;

// Internal API
// -----------------------------------------------------------------

void bxr_mouse_setup(void);
void bxr_mouse_begin_frame(void);
void bxr_mouse_shutdown(void);
void bxr_mouse_button_down(bxr_mouse_e button, bxr_mouse_click_e click);

void bxr_mouse_button_up(bxr_mouse_e button);
void bxr_mouse_motion(float x, float y);
void bxr_mouse_wheel_motion(float x, float y);

// Public API
// -----------------------------------------------------------------

float bxr_mouse_x(void);
float bxr_mouse_y(void);

float bxr_mouse_prev_x(void);
float bxr_mouse_prev_y(void);

bool bxr_mouse_held(bxr_mouse_e button);

bool bxr_mouse_just_pressed(bxr_mouse_e button);
bool bxr_mouse_just_released(bxr_mouse_e button);

bool bxr_mouse_double_clicked(bxr_mouse_e button);

Uint64 bxr_mouse_held_time(bxr_mouse_e button);

float bxr_mouse_wheel_motion_x(void);
float bxr_mouse_wheel_motion_y(void);

void bxr_mouse_hide(bool hide);
bool bxr_mouse_is_hidden(void);

// TODO add mouse grab, for that the we need the window from the app context

#endif // BXR_MOUSE_H_
