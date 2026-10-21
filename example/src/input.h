/**
 * @file input.h
 *
 * Mimic an NES controller input system with an easy to use API for checking
 * button states.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <boxer/bxr.h>

typedef enum input_button_e
{
  INPUT_BUTTON_A = 0,
  INPUT_BUTTON_B,
  INPUT_BUTTON_START,
  INPUT_BUTTON_SELECT,
  INPUT_BUTTON_UP,
  INPUT_BUTTON_DOWN,
  INPUT_BUTTON_LEFT,
  INPUT_BUTTON_RIGHT,
  INPUT_BUTTON_SIZE
} input_button_e;

/**
 * returns true if any button was just pressed in the current frame. False
 * otherwise.
 */
bool input_any_just_pressed(void);

/**
 * returns true if any button is being held down in the current frame. False
 * otherwise.
 */
bool input_just_pressed(input_button_e button);

/**
 * returns true if any button was just released in the current frame. False
 * otherwise.
 */
bool input_held(input_button_e button);

#endif // INPUT_H_
