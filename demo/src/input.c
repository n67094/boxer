#include <boxer/bxr.h>

#include "input.h"

bool
input_any_just_pressed(void)
{
  return bxr_gamepad_any_just_pressed(0) || bxr_key_any_just_pressed();
}

bool
input_just_pressed(input_button_e button)
{
  BXR_ASSERT(button >= 0 && button < INPUT_BUTTON_SIZE);

  switch (button) {
  case INPUT_BUTTON_A:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_EAST)
           || bxr_key_just_pressed(BXR_KEY_K);
  case INPUT_BUTTON_B:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_SOUTH)
           || bxr_key_just_pressed(BXR_KEY_J);
  case INPUT_BUTTON_START:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_START)
           || bxr_key_just_pressed(BXR_KEY_RETURN);
  case INPUT_BUTTON_SELECT:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_BACK)
           || bxr_key_just_pressed(BXR_KEY_BACKSPACE);
  case INPUT_BUTTON_UP:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_UP)
           || bxr_key_just_pressed(BXR_KEY_W);
  case INPUT_BUTTON_DOWN:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_DOWN)
           || bxr_key_just_pressed(BXR_KEY_S);
  case INPUT_BUTTON_LEFT:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_LEFT)
           || bxr_key_just_pressed(BXR_KEY_A);
  case INPUT_BUTTON_RIGHT:
    return bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_RIGHT)
           || bxr_key_just_pressed(BXR_KEY_D);
  default:
    return false;
  }
}

bool
input_held(input_button_e button)
{
  BXR_ASSERT(button >= 0 && button < INPUT_BUTTON_SIZE);

  switch (button) {
  case INPUT_BUTTON_A:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_EAST)
           || bxr_key_held(BXR_KEY_K);
  case INPUT_BUTTON_B:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_SOUTH)
           || bxr_key_held(BXR_KEY_J);
  case INPUT_BUTTON_START:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_START)
           || bxr_key_held(BXR_KEY_RETURN);
  case INPUT_BUTTON_SELECT:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_START)
           || bxr_key_held(BXR_KEY_BACKSPACE);
  case INPUT_BUTTON_UP:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_DPAD_UP)
           || bxr_key_held(BXR_KEY_W);
  case INPUT_BUTTON_DOWN:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_DPAD_DOWN)
           || bxr_key_held(BXR_KEY_S);
  case INPUT_BUTTON_LEFT:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_DPAD_LEFT)
           || bxr_key_held(BXR_KEY_A);
  case INPUT_BUTTON_RIGHT:
    return bxr_gamepad_held(0, BXR_GAMEPAD_BUTTON_DPAD_RIGHT)
           || bxr_key_held(BXR_KEY_D);
  default:
    return false;
  }
}
