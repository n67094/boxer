#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_gamepad.h"

typedef struct _bxr_gamepad_button_state_s
{
  bool is_down;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at;
} _bxr_gamepad_button_state_t;

typedef struct _bxr_gamepad_s
{
  SDL_Gamepad *gamepad;
  SDL_JoystickID id;
  const char *serial;

  _bxr_gamepad_button_state_t buttons[BXR_GAMEPAD_BUTTON_SIZE];

  bool just_pressed_acc[BXR_GAMEPAD_BUTTON_SIZE];
  bool just_released_acc[BXR_GAMEPAD_BUTTON_SIZE];

  float axis[BXR_GAMEPAD_AXIS_SIZE];
  float axis_prev[BXR_GAMEPAD_AXIS_SIZE];
} _bxr_gamepad_t;

static Uint32 _initialized                       = 0;
static _bxr_gamepad_t _gamepads[BXR_GAMEPAD_MAX] = { 0 };
static float _deadzone                           = BXR_GAMEPAD_DEADZONE;

void
bxr_gamepad_setup(void)
{
  SDL_assert(_initialized == 0);

  _initialized = BXR_INIT_COOKIE;
  _deadzone    = BXR_GAMEPAD_DEADZONE;
}

void
bxr_gamepad_listen(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  // Handle disconnected gamepads
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (_gamepads[i].gamepad == NULL) {
      continue;
    }

    // Remove disconnected gamepads
    if (!SDL_GamepadConnected(_gamepads[i].gamepad)) {
      SDL_CloseGamepad(_gamepads[i].gamepad);
      _gamepads[i].gamepad = NULL;
      _gamepads[i].id      = 0;
      _gamepads[i].serial  = NULL;
    }
  }

  int gamepad_count           = 0;
  SDL_JoystickID *gamepad_ids = SDL_GetGamepads(&gamepad_count);

  // Handle new gamepads
  for (int i = 0; i < gamepad_count; ++i) {
    SDL_JoystickID id = gamepad_ids[i];

    // Try to connect the gamepad
    SDL_Gamepad *gamepad = SDL_GetGamepadFromID(id);
    if (gamepad == NULL) { // Not connected yet try to open it
      gamepad = SDL_OpenGamepad(id);
      if (gamepad == NULL) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Failed to open gamepad with id %d (error: %s)",
                    id,
                    SDL_GetError());
        continue;
      }
    }

    // If the gamepad is already in the list continue
    bool found = false;
    for (int j = 0; j < BXR_GAMEPAD_MAX; ++j) {
      const char *serial = SDL_GetGamepadSerial(gamepad);
      if (_gamepads[j].id == id
          || (serial && _gamepads[j].serial
              && SDL_strcmp(_gamepads[j].serial, serial) == 0)) {
        found = true;
        break;
      }
    }
    if (found) {
      continue;
    }

    // New gamepad map it to the first empty slot
    for (int j = 0; j < BXR_GAMEPAD_MAX; ++j) {
      if (_gamepads[j].gamepad == NULL) {
        // If the slot is empty use it for the new gamepad
        SDL_SetGamepadPlayerIndex(gamepad, j);

        _gamepads[j].gamepad = gamepad;
        _gamepads[j].id      = id;
        _gamepads[j].serial  = SDL_GetGamepadSerial(gamepad);

        SDL_memset(_gamepads[j].buttons, 0, sizeof(_gamepads[j].buttons));
        SDL_memset(_gamepads[j].axis, 0, sizeof(_gamepads[j].axis));
        SDL_memset(_gamepads[j].axis_prev, 0, sizeof(_gamepads[j].axis_prev));
        break;
      }
    }

    SDL_free(gamepad_ids);
  }
}

void
bxr_gamepad_begin_frame(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (_gamepads[i].gamepad == NULL) {
      continue;
    }

    // Update just pressed/released accumulators
    for (int j = 0; j < BXR_GAMEPAD_BUTTON_SIZE; ++j) {
      _gamepads[i].just_pressed_acc[j]  = _gamepads[i].buttons[j].just_pressed;
      _gamepads[i].just_released_acc[j] = _gamepads[i].buttons[j].just_released;

      _gamepads[i].buttons[j].just_pressed  = false;
      _gamepads[i].buttons[j].just_released = false;
    }

    // Update previous axis values
    SDL_memcpy(_gamepads[i].axis_prev,
               _gamepads[i].axis,
               sizeof(_gamepads[i].axis_prev));
  }
}

void
bxr_gamepad_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  // Close gamepads
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (_gamepads[i].gamepad != NULL) {
      SDL_CloseGamepad(_gamepads[i].gamepad);
      _gamepads[i].gamepad = NULL;
    }
  }

  _initialized = 0;
}

void
bxr_gamepad_button_down(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  if (!_gamepads[index].buttons[button].is_down) {
    _gamepads[index].buttons[button].pressed_at   = SDL_GetTicks();
    _gamepads[index].buttons[button].just_pressed = true;
  }

  _gamepads[index].buttons[button].is_down = true;
}

void
bxr_gamepad_button_up(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  if (_gamepads[index].buttons[button].is_down) {
    _gamepads[index].buttons[button].just_released = true;
  }

  _gamepads[index].buttons[button].pressed_at = 0;
  _gamepads[index].buttons[button].is_down    = false;
}

void
bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  value /= 32768.0f; // Convert to (-1.0f to 1.0f)

  if (SDL_fabsf(value) < _deadzone) { // Apply deadzone
    value = 0.0f;
  }

  _gamepads[index].axis[axis] = value;
}

int
bxr_gamepad_count(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  int count = 0;
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (_gamepads[i].gamepad != NULL) {
      count++;
    }
  }

  return count;
}

void
bxr_gamepad_set_deadzone(float deadzone)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(deadzone >= 0.0f && deadzone <= 1.0f);

  _deadzone = deadzone;
}

float
bxr_gamepad_get_deadzone()
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  return _deadzone;
}

bool
bxr_gamepad_connected(int index)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (_gamepads[index].gamepad == NULL) {
    return false;
  }

  return SDL_GamepadConnected(_gamepads[index].gamepad);
}

const char *
bxr_gamepad_name(int index)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return NULL;
  }

  return SDL_GetGamepadName(_gamepads[index].gamepad);
}

const char *
bxr_gamepad_serial(int index)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return NULL;
  }

  return SDL_GetGamepadSerial(_gamepads[index].gamepad);
}

bool
bxr_gamepad_held(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return _gamepads[index].buttons[button].is_down;
}

bool
bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return _gamepads[index].just_pressed_acc[button];
}

bool
bxr_gamepad_just_released(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return _gamepads[index].just_released_acc[button];
}

Uint64
bxr_gamepad_held_time(int index, bxr_gamepad_button_e button)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0;
  }

  if (_gamepads[index].buttons[button].is_down) {
    return SDL_GetTicks() - _gamepads[index].buttons[button].pressed_at;
  } else {
    return 0;
  }
}

float
bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0.0f;
  }

  return _gamepads[index].axis[axis];
}

float
bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(index >= 0 && index < BXR_GAMEPAD_MAX);
  SDL_assert(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0.0f;
  }

  return _gamepads[index].axis_prev[axis];
}

bool
bxr_gamepad_rumble(int index,
                   Uint16 low_frequency,
                   Uint16 high_frequency,
                   Uint32 duration_ms)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  if (_gamepads[index].gamepad == NULL) {
    return false;
  }

  low_frequency  = SDL_clamp(low_frequency, 0, 100);
  high_frequency = SDL_clamp(high_frequency, 0, 100);

  // Convert from percentage (0-100) to hex value (0x0000-0xFFFF)
  // 0xFFFF is max rumble value (from SDL3 docs)
  int low_frequency_hex  = (0xFFFF / 100) * low_frequency;
  int high_frequency_hex = (0xFFFF / 100) * high_frequency;

  // Start rumble
  return SDL_RumbleGamepad(_gamepads[index].gamepad,
                           low_frequency_hex,
                           high_frequency_hex,
                           duration_ms);
}
