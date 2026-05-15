/**
 * File `bxr_gamepad.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Gamepad Input Handling
 */

#ifndef BXR_GAMEPAD_H_
#define BXR_GAMEPAD_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

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

/**
 * ## Internal API
 */

/**
 * Initializes the gamepad subsystem. Must be called before using any other
 * gamepad functions.
 */
BXR_API void bxr_gamepad_setup(void);

/**
 * Shuts down the gamepad subsystem. Must be called when the application is
 * shutting down.
 */
BXR_API void bxr_gamepad_shutdown(void);

/**
 * Listens for gamepad events and updates the internal state of the gamepads.
 * Must be called every frame to ensure that the gamepad state is updated.
 */
BXR_API void bxr_gamepad_listen(void);

/**
 * Update the gamepad state for the current frame. Must be called at the
 * beginning of each frame before processing input.
 */
BXR_API void bxr_gamepad_begin_frame(void);

/**
 * Handles a gamepad button down event. Must be called when a gamepad button
 * down event is received from SDL.
 *
 * `index` is the index of the gamepad that the event occurred on.
 *
 * `button` is the button that was pressed.
 */
BXR_API void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button);

/**
 * Handles a gamepad button up event. Must be called when a gamepad button up
 * event is received from SDL.
 *
 * `index` is the index of the gamepad that the event occurred on.
 *
 * `button` is the button that was released.
 */
BXR_API void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button);

/**
 * Handles a gamepad axis motion event. Must be called when a gamepad axis
 * motion event is received from SDL.
 *
 * `index` is the index of the gamepad that the event occurred on.
 *
 * `axis` is the axis that was moved.
 *
 * `value` is the new value of the axis, normalized to the range [-1, 1].
 */
BXR_API void
bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value);

/**
 * ## Public API
 */

/**
 * Get the number of connected gamepads.
 *
 * `return` the number of connected gamepads.
 */
BXR_API int bxr_gamepad_count(void);

/**
 * Set the deadzone for gamepad axes.
 *
 * `deadzone` is the deadzone to set.
 */
BXR_API void bxr_gamepad_set_deadzone(float deadzone);

/**
 * Get the current deadzone for gamepad axes.
 *
 * `return` the current deadzone for gamepad axes.
 */
BXR_API float bxr_gamepad_get_deadzone(void);

/**
 * Check if a gamepad is connected.
 *
 * `index` is the index of the gamepad to check.
 *
 * `return` true if the gamepad is connected, false otherwise.
 */
BXR_API bool bxr_gamepad_connected(int index);

/**
 * Get the name of a gamepad.
 *
 * `index` is the index of the gamepad to get the name of.
 *
 * `return` the name of the gamepad, or NULL if the gamepad is not connected.
 */
BXR_API const char *bxr_gamepad_name(int index);

/**
 * Get the serial number of a gamepad.
 *
 * `index` is the index of the gamepad to get the serial number of.
 *
 * `return` the serial number of the gamepad, or NULL if the gamepad is not
 * connected.
 */
BXR_API const char *bxr_gamepad_serial(int index);

/**
 * Check if a gamepad button is currently held down.
 *
 * `index` is the index of the gamepad to check.
 *
 * `button` is the button to check.
 *
 * `return` true if the button is currently held down, false otherwise.
 */
BXR_API bool bxr_gamepad_held(int index, bxr_gamepad_button_e button);

/**
 * Check if a gamepad button was just pressed this frame.
 *
 * `index` is the index of the gamepad to check.
 *
 * `button` is the button to check.
 *
 * `return` true if the button was just pressed this frame, false otherwise.
 */
BXR_API bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button);

/**
 * Check if a gamepad button was just released this frame.
 *
 * `index` is the index of the gamepad to check.
 *
 * `button` is the button to check.
 *
 * `return` true if the button was just released this frame, false otherwise.
 */
BXR_API bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button);

/**
 * Get the amount of time a gamepad button has been held down.
 *
 * `index` is the index of the gamepad to check.
 *
 * `button` is the button to check.
 *
 * `return` the amount of time the button has been held down in milliseconds.
 */
BXR_API Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button);

/**
 * Get the current value of a gamepad axis.
 *
 * `index` is the index of the gamepad to check.
 *
 * `axis` is the axis to check.
 *
 * `return` the current value of the axis.
 */
BXR_API float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis);

/**
 * Get the value of a gamepad axis in the previous frame.
 *
 * `index` is the index of the gamepad to check.
 *
 * `axis` is the axis to check.
 *
 * `return` the value of the axis in the previous frame.
 */
BXR_API float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis);

/**
 * Set the rumble state of a gamepad.
 *
 * `index` is the index of the gamepad to set the rumble state of.
 *
 * `low_frequency` is the intensity of the low frequency rumble motor.
 *
 * `high_frequency` is the intensity of the high frequency rumble motor.
 *
 * `duration_ms` is the duration of the rumble effect in milliseconds.
 *
 * `return` true if the rumble state was successfully set, false otherwise.
 */
BXR_API bool bxr_gamepad_rumble(int index,
                                Uint16 low_frequency,
                                Uint16 high_frequency,
                                Uint32 duration_ms);

#endif // BXR_GAMEPAD_H_
