/**
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * @brief Gamepad input handling.
 *
 * @details This module provides functions for handling up to BXR_GAMEPAD_MAX
 * gamepads.
 *
 * Any gamepad supported by SDL will be recognized and handled by this module.
 */

#ifndef BXR_GAMEPAD_H_
#define BXR_GAMEPAD_H_

#include <SDL3/SDL.h>

#define BXR_GAMEPAD_MAX 4
#define BXR_GAMEPAD_DEADZONE 0.15f

typedef enum
{
  BXR_GAMEPAD_BUTTON_UNKNOWN = -1,
  BXR_GAMEPAD_BUTTON_SOUTH,
  BXR_GAMEPAD_BUTTON_EAST,
  BXR_GAMEPAD_BUTTON_WEST,
  BXR_GAMEPAD_BUTTON_NORTH,
  BXR_GAMEPAD_BUTTON_BACK,
  BXR_GAMEPAD_BUTTON_GUIDE,
  BXR_GAMEPAD_BUTTON_START,
  BXR_GAMEPAD_BUTTON_LEFT_STICK,
  BXR_GAMEPAD_BUTTON_RIGHT_STICK,
  BXR_GAMEPAD_BUTTON_LEFT_SHOULDER,
  BXR_GAMEPAD_BUTTON_RIGHT_SHOULDER,
  BXR_GAMEPAD_BUTTON_DPAD_UP,
  BXR_GAMEPAD_BUTTON_DPAD_DOWN,
  BXR_GAMEPAD_BUTTON_DPAD_LEFT,
  BXR_GAMEPAD_BUTTON_DPAD_RIGHT,
  BXR_GAMEPAD_BUTTON_MISC1,
  BXR_GAMEPAD_BUTTON_RIGHT_PADDLE_1,
  BXR_GAMEPAD_BUTTON_LEFT_PADDLE_1,
  BXR_GAMEPAD_BUTTON_RIGHT_PADDLE_2,
  BXR_GAMEPAD_BUTTON_LEFT_PADDLE_2,
  BXR_GAMEPAD_BUTTON_TOUCHPAD,
  BXR_GAMEPAD_BUTTON_MISC2,
  BXR_GAMEPAD_BUTTON_MISC3,
  BXR_GAMEPAD_BUTTON_MISC4,
  BXR_GAMEPAD_BUTTON_MISC5,
  BXR_GAMEPAD_BUTTON_MISC6,
  BXR_GAMEPAD_BUTTON_SIZE,
} bxr_gamepad_button_e;

typedef enum
{
  BXR_GAMEPAD_AXIS_INVALID = -1,
  BXR_GAMEPAD_AXIS_LEFTX,
  BXR_GAMEPAD_AXIS_LEFTY,
  BXR_GAMEPAD_AXIS_RIGHTX,
  BXR_GAMEPAD_AXIS_RIGHTY,
  BXR_GAMEPAD_AXIS_LEFT_TRIGGER,
  BXR_GAMEPAD_AXIS_RIGHT_TRIGGER,
  BXR_GAMEPAD_AXIS_SIZE,
} bxr_gamepad_axis_e;

// Internal API
// -----------------------------------------------------------------

void bxr_gamepad_setup(void);
void bxr_gamepad_listen(void);
void bxr_gamepad_begin_frame(void);
void bxr_gamepad_shutdown(void);
void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button);
void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button);
void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value);

// Public API
// -----------------------------------------------------------------

int bxr_gamepad_count(void);

void bxr_gamepad_set_deadzone(float deadzone);
float bxr_gamepad_get_deadzone(void);

bool bxr_gamepad_connected(int index);

const char *bxr_gamepad_name(int index);

const char *bxr_gamepad_serial(int index);

bool bxr_gamepad_held(int index, bxr_gamepad_button_e button);

bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button);
bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button);

Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button);

float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis);
float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis);

bool bxr_gamepad_rumble(int index,
                        Uint16 low_frequency,
                        Uint16 high_frequency,
                        Uint32 duration_ms);

#endif // BXR_GAMEPAD_H_
