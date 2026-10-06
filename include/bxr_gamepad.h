/**
 * File `bxr_gamepad.h`.
 *
 * Gamepad input subsystem.
 *
 * Up to `BXR_GAMEPAD_MAX` gamepads are tracked.
 *
 * Each one is identified by an `index` in [0, `BXR_GAMEPAD_MAX`), the slot
 * (player index) it was assigned when connected.
 *
 * Boxer polls connections and feeds events itself.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
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
 * Initialize the gamepad subsystem.
 */
void bxr_gamepad_setup(void);

/**
 * Close every open gamepad.
 */
void bxr_gamepad_teardown(void);

/**
 * Close disconnected gamepads and assign newly connected ones to the first
 * free slot, resetting its button and axis state.
 */
void bxr_gamepad_listen(void);

/**
 * Update the gamepad state for the new frame. Must be called once at the
 * beginning of each frame before processing input.
 */
void bxr_gamepad_begin_frame(void);

/**
 * Record a button press.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button pressed, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 */
void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button);

/**
 * Record a button release.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button released, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 */
void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button);

/**
 * Record an axis motion.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `axis` axis moved, in [0, `BXR_GAMEPAD_AXIS_SIZE`).
 * `value` new axis value, in [-32768, 32767].
 */
void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, Sint16 value);

/**
 * ## Public API
 */

/**
 * Get the number of gamepads currently connected.
 *
 * `return` the count, in [0, `BXR_GAMEPAD_MAX`].
 */
BXR_API int bxr_gamepad_count(void);

/**
 * Set the deadzone shared by every gamepad axis.
 *
 * `deadzone` normalized magnitude below which an axis reads 0, in [0, 1].
 */
BXR_API void bxr_gamepad_set_deadzone(float deadzone);

/**
 * Get the deadzone shared by every gamepad axis.
 *
 * `return` the deadzone, in [0, 1].
 */
BXR_API float bxr_gamepad_get_deadzone(void);

/**
 * Check if a gamepad is connected.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 *
 * `return` true if a gamepad is assigned to the slot and still connected,
 * false otherwise.
 */
BXR_API bool bxr_gamepad_connected(int index);

/**
 * Get the name of a gamepad.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 *
 * `return` the name, or `NULL` if the gamepad is not connected or has no name.
 *
 * **Note**: the string is owned by Boxer, do not free it.
 */
BXR_API const char *bxr_gamepad_name(int index);

/**
 * Get the serial number of a gamepad.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 *
 * `return` the serial number, or `NULL` if the gamepad is not connected.
 *
 * **Note**: the string is owned by Boxer, do not free it.
 */
BXR_API const char *bxr_gamepad_serial(int index);

/**
 * Check if a button is held down.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button to check, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 *
 * `return` true if held, false otherwise or if the gamepad is not connected.
 */
BXR_API bool bxr_gamepad_held(int index, bxr_gamepad_button_e button);

/**
 * Check if any button was pressed this frame.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 *
 * `return` true if at least one button was pressed since the previous frame,
 * false otherwise or if the gamepad is not connected.
 */
BXR_API bool bxr_gamepad_any_just_pressed(int index);

/**
 * Check if a button was pressed this frame.
 *
 * True for exactly one frame per press.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button to check, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 *
 * `return` true if pressed since the previous frame, false otherwise or if the
 * gamepad is not connected.
 */
BXR_API bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button);

/**
 * Check if a button was released this frame.
 *
 * True for exactly one frame per release.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button to check, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 *
 * `return` true if released since the previous frame, false otherwise or if
 * the gamepad is not connected.
 */
BXR_API bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button);

/**
 * Get how long a button has been held down.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `button` button to check, in [0, `BXR_GAMEPAD_BUTTON_SIZE`).
 *
 * `return` the time in milliseconds since the press, or 0 if the button is
 * not held or the gamepad is not connected.
 */
BXR_API Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button);

/**
 * Get the current value of an axis, after the deadzone is applied.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`) .
 * `axis` axis to read, in [0, `BXR_GAMEPAD_AXIS_SIZE`).
 *
 * `return` the value in [-1, 1) for sticks (positive is right/down) and [0, 1)
 * for triggers, 0 inside the deadzone or if the gamepad is not connected.
 */
BXR_API float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis);

/**
 * Get the value an axis had at the end of the previous frame.
 *
 * Compare with `bxr_gamepad_axis()` to detect a change this frame.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `axis` axis to read, in [0, `BXR_GAMEPAD_AXIS_SIZE`).
 *
 * `return` the value, same range as `bxr_gamepad_axis()`, or 0 if the gamepad
 * is not connected.
 */
BXR_API float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis);

/**
 * Start a rumble effect, replacing any effect in progress.
 *
 * `index` slot of the gamepad, in [0, `BXR_GAMEPAD_MAX`).
 * `low_frequency` intensity of the low frequency motor, in [0, 0xFFFF], same
 * scale as `SDL_RumbleGamepad()`; 0 turns the motor off.
 * `high_frequency` intensity of the high frequency motor, in [0, 0xFFFF], same
 * scale as `SDL_RumbleGamepad()`; 0 turns the motor off.
 * `duration_ms` duration of the effect in milliseconds; 0 stops the rumble.
 *
 * `return` true on success, false on failure; the reason is available from
 * `SDL_GetError()`.
 */
BXR_API bool bxr_gamepad_rumble(int index,
                                Uint16 low_frequency,
                                Uint16 high_frequency,
                                Uint32 duration_ms);

#endif // BXR_GAMEPAD_H_
