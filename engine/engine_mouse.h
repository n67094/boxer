/**
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * @brief Mouse input handling.
 *
 * @details This module provides functions for handling mouse input, including
 * mouse position, button states, wheel motion, and cursor visibility.
 */

#ifndef ENGINE_MOUSE_H_
#define ENGINE_MOUSE_H_

#include <SDL3/SDL.h>

#include "engine_context.h"

typedef enum
{
  ENGINE_MOUSE_NONE = 0,
  ENGINE_MOUSE_SINGLE,
  ENGINE_MOUSE_DOUBLE,
} engine_mouse_click_e;

typedef enum
{
  ENGINE_MOUSE_UNKNOWN = 0,
  ENGINE_MOUSE_LEFT    = SDL_BUTTON_LEFT,
  ENGINE_MOUSE_MIDDLE  = SDL_BUTTON_MIDDLE,
  ENGINE_MOUSE_RIGHT   = SDL_BUTTON_RIGHT,
  ENGINE_MOUSE_X1      = SDL_BUTTON_X1,
  ENGINE_MOUSE_X2      = SDL_BUTTON_X2,
  ENGINE_MOUSE_SIZE    = 6,
} engine_mouse_e;

// Internal API
// -----------------------------------------------------------------

void engine_mouse_setup(void);
void engine_mouse_begin_frame(void);
void engine_mouse_shutdown(void);
void engine_mouse_button_down(engine_mouse_e button,
                              engine_mouse_click_e click);

void engine_mouse_button_up(engine_mouse_e button);
void engine_mouse_motion(float x, float y);
void engine_mouse_wheel_motion(float x, float y);

// Public API
// -----------------------------------------------------------------

float engine_mouse_x(void);
float engine_mouse_y(void);

float engine_mouse_prev_x(void);
float engine_mouse_prev_y(void);

bool engine_mouse_held(engine_mouse_e button);

bool engine_mouse_just_pressed(engine_mouse_e button);
bool engine_mouse_just_released(engine_mouse_e button);

bool engine_mouse_double_clicked(engine_mouse_e button);

Uint64 engine_mouse_held_time(engine_mouse_e button);

float engine_mouse_wheel_motion_x(void);
float engine_mouse_wheel_motion_y(void);

void engine_mouse_hide(bool hide);
bool engine_mouse_is_hidden(void);

// TODO add mouse grab, for that the we need the window from the app context

#endif // ENGINE_MOUSE_H_
