/**
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * @brief Gamepad input handling.
 *
 * @details This module provides functions for handling up to ENGINE_GAMEPAD_MAX
 * gamepads.
 *
 * Any gamepad supported by SDL will be recognized and handled by this module.
 */

#ifndef ENGINE_GAMEPAD_H_
#define ENGINE_GAMEPAD_H_

#include <SDL3/SDL.h>

#define ENGINE_GAMEPAD_MAX 4
#define ENGINE_GAMEPAD_DEADZONE 0.15f

typedef enum
{
  ENGINE_GAMEPAD_BUTTON_UNKNOWN = -1,
  ENGINE_GAMEPAD_BUTTON_SOUTH,
  ENGINE_GAMEPAD_BUTTON_EAST,
  ENGINE_GAMEPAD_BUTTON_WEST,
  ENGINE_GAMEPAD_BUTTON_NORTH,
  ENGINE_GAMEPAD_BUTTON_BACK,
  ENGINE_GAMEPAD_BUTTON_GUIDE,
  ENGINE_GAMEPAD_BUTTON_START,
  ENGINE_GAMEPAD_BUTTON_LEFT_STICK,
  ENGINE_GAMEPAD_BUTTON_RIGHT_STICK,
  ENGINE_GAMEPAD_BUTTON_LEFT_SHOULDER,
  ENGINE_GAMEPAD_BUTTON_RIGHT_SHOULDER,
  ENGINE_GAMEPAD_BUTTON_DPAD_UP,
  ENGINE_GAMEPAD_BUTTON_DPAD_DOWN,
  ENGINE_GAMEPAD_BUTTON_DPAD_LEFT,
  ENGINE_GAMEPAD_BUTTON_DPAD_RIGHT,
  ENGINE_GAMEPAD_BUTTON_MISC1,
  ENGINE_GAMEPAD_BUTTON_RIGHT_PADDLE_1,
  ENGINE_GAMEPAD_BUTTON_LEFT_PADDLE_1,
  ENGINE_GAMEPAD_BUTTON_RIGHT_PADDLE_2,
  ENGINE_GAMEPAD_BUTTON_LEFT_PADDLE_2,
  ENGINE_GAMEPAD_BUTTON_TOUCHPAD,
  ENGINE_GAMEPAD_BUTTON_MISC2,
  ENGINE_GAMEPAD_BUTTON_MISC3,
  ENGINE_GAMEPAD_BUTTON_MISC4,
  ENGINE_GAMEPAD_BUTTON_MISC5,
  ENGINE_GAMEPAD_BUTTON_MISC6,
  ENGINE_GAMEPAD_BUTTON_SIZE,
} engine_gamepad_button_e;

typedef enum
{
  ENGINE_GAMEPAD_AXIS_INVALID = -1,
  ENGINE_GAMEPAD_AXIS_LEFTX,
  ENGINE_GAMEPAD_AXIS_LEFTY,
  ENGINE_GAMEPAD_AXIS_RIGHTX,
  ENGINE_GAMEPAD_AXIS_RIGHTY,
  ENGINE_GAMEPAD_AXIS_LEFT_TRIGGER,
  ENGINE_GAMEPAD_AXIS_RIGHT_TRIGGER,
  ENGINE_GAMEPAD_AXIS_SIZE,
} engine_gamepad_axis_e;

// Internal API
// -----------------------------------------------------------------

void engine_gamepad_setup(void);
void engine_gamepad_listen(void);
void engine_gamepad_begin_frame(void);
void engine_gamepad_shutdown(void);
void engine_gamepad_button_down(int index, engine_gamepad_button_e button);
void engine_gamepad_button_up(int index, engine_gamepad_button_e button);
void
engine_gamepad_axis_motion(int index, engine_gamepad_axis_e axis, float value);

// Public API
// -----------------------------------------------------------------

int engine_gamepad_count(void);

void engine_gamepad_set_deadzone(float deadzone);
float engine_gamepad_get_deadzone(void);

bool engine_gamepad_connected(int index);

const char *engine_gamepad_name(int index);

const char *engine_gamepad_serial(int index);

bool engine_gamepad_held(int index, engine_gamepad_button_e button);

bool engine_gamepad_just_pressed(int index, engine_gamepad_button_e button);
bool engine_gamepad_just_released(int index, engine_gamepad_button_e button);

Uint64 engine_gamepad_held_time(int index, engine_gamepad_button_e button);

float engine_gamepad_axis(int index, engine_gamepad_axis_e axis);
float engine_gamepad_axis_prev(int index, engine_gamepad_axis_e axis);

bool engine_gamepad_rumble(int index,
                           Uint16 low_frequency,
                           Uint16 high_frequency,
                           Uint32 duration_ms);

#endif // ENGINE_GAMEPAD_H_
