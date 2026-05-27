#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_defs.h"
#include "bxr_gamepad.h"
#include "bxr_log.h"
#include "bxr_mem.h"

typedef struct bxr_gamepad_button_state_s_
{
  bool is_down;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at;
} bxr_gamepad_button_state_t_;

typedef struct bxr_gamepad_s_
{
  SDL_Gamepad *gamepad;
  SDL_JoystickID id;
  const char *serial;

  bxr_gamepad_button_state_t_ buttons[BXR_GAMEPAD_BUTTON_SIZE];

  bool just_pressed_acc[BXR_GAMEPAD_BUTTON_SIZE];
  bool just_released_acc[BXR_GAMEPAD_BUTTON_SIZE];

  float axis[BXR_GAMEPAD_AXIS_SIZE];
  float axis_prev[BXR_GAMEPAD_AXIS_SIZE];
} bxr_gamepad_t_;

static Uint32 initialized_                       = 0;
static bxr_gamepad_t_ gamepads_[BXR_GAMEPAD_MAX] = { 0 };
static float deadzone_                           = BXR_GAMEPAD_DEADZONE;

void
bxr_gamepad_setup(void)
{
  BXR_ASSERT(initialized_ == 0);

  initialized_ = BXR_INIT_COOKIE;
  deadzone_    = BXR_GAMEPAD_DEADZONE;
}

void
bxr_gamepad_listen(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  // Handle disconnected gamepads
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (gamepads_[i].gamepad == NULL) {
      continue;
    }

    // Remove disconnected gamepads
    if (!SDL_GamepadConnected(gamepads_[i].gamepad)) {
      SDL_CloseGamepad(gamepads_[i].gamepad);
      gamepads_[i].gamepad = NULL;
      gamepads_[i].id      = 0;
      gamepads_[i].serial  = NULL;
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
        BXR_LOG_ERROR("Failed to open gamepad with id %d (error: %s)",
                      id,
                      SDL_GetError());
        continue;
      }
    }

    // If the gamepad is already in the list continue
    bool found = false;
    for (int j = 0; j < BXR_GAMEPAD_MAX; ++j) {
      const char *serial = SDL_GetGamepadSerial(gamepad);
      if (gamepads_[j].id == id
          || (serial && gamepads_[j].serial
              && SDL_strcmp(gamepads_[j].serial, serial) == 0)) {
        found = true;
        break;
      }
    }
    if (found) {
      continue;
    }

    // New gamepad map it to the first empty slot
    for (int j = 0; j < BXR_GAMEPAD_MAX; ++j) {
      if (gamepads_[j].gamepad == NULL) {
        // If the slot is empty use it for the new gamepad
        SDL_SetGamepadPlayerIndex(gamepad, j);

        gamepads_[j].gamepad = gamepad;
        gamepads_[j].id      = id;
        gamepads_[j].serial  = SDL_GetGamepadSerial(gamepad);

        BXR_MEMSET(gamepads_[j].buttons, 0, sizeof(gamepads_[j].buttons));
        BXR_MEMSET(gamepads_[j].axis, 0, sizeof(gamepads_[j].axis));
        BXR_MEMSET(gamepads_[j].axis_prev, 0, sizeof(gamepads_[j].axis_prev));
        break;
      }
    }
  }

  SDL_free(gamepad_ids);
}

void
bxr_gamepad_begin_frame(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (gamepads_[i].gamepad == NULL) {
      continue;
    }

    // Update just pressed/released accumulators
    for (int j = 0; j < BXR_GAMEPAD_BUTTON_SIZE; ++j) {
      gamepads_[i].just_pressed_acc[j]  = gamepads_[i].buttons[j].just_pressed;
      gamepads_[i].just_released_acc[j] = gamepads_[i].buttons[j].just_released;

      gamepads_[i].buttons[j].just_pressed  = false;
      gamepads_[i].buttons[j].just_released = false;
    }

    // Update previous axis values
    BXR_MEMCPY(gamepads_[i].axis_prev,
               gamepads_[i].axis,
               sizeof(gamepads_[i].axis_prev));
  }
}

void
bxr_gamepad_teardown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  // Close gamepads
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (gamepads_[i].gamepad != NULL) {
      SDL_CloseGamepad(gamepads_[i].gamepad);
      gamepads_[i].gamepad = NULL;
    }
  }

  initialized_ = 0;
}

void
bxr_gamepad_button_down(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  if (!gamepads_[index].buttons[button].is_down) {
    gamepads_[index].buttons[button].pressed_at   = SDL_GetTicks();
    gamepads_[index].buttons[button].just_pressed = true;
  }

  gamepads_[index].buttons[button].is_down = true;
}

void
bxr_gamepad_button_up(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  if (gamepads_[index].buttons[button].is_down) {
    gamepads_[index].buttons[button].just_released = true;
  }

  gamepads_[index].buttons[button].pressed_at = 0;
  gamepads_[index].buttons[button].is_down    = false;
}

void
bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return;
  }

  value /= 32768.0f; // Convert to (-1.0f to 1.0f)

  if (SDL_fabsf(value) < deadzone_) { // Apply deadzone
    value = 0.0f;
  }

  gamepads_[index].axis[axis] = value;
}

int
bxr_gamepad_count(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  int count = 0;
  for (int i = 0; i < BXR_GAMEPAD_MAX; ++i) {
    if (gamepads_[i].gamepad != NULL) {
      count++;
    }
  }

  return count;
}

void
bxr_gamepad_set_deadzone(float deadzone)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(deadzone >= 0.0f && deadzone <= 1.0f);

  deadzone_ = deadzone;
}

float
bxr_gamepad_get_deadzone()
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return deadzone_;
}

bool
bxr_gamepad_connected(int index)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (gamepads_[index].gamepad == NULL) {
    return false;
  }

  return SDL_GamepadConnected(gamepads_[index].gamepad);
}

const char *
bxr_gamepad_name(int index)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return NULL;
  }

  return SDL_GetGamepadName(gamepads_[index].gamepad);
}

const char *
bxr_gamepad_serial(int index)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);

  if (!bxr_gamepad_connected(index)) {
    return NULL;
  }

  return SDL_GetGamepadSerial(gamepads_[index].gamepad);
}

bool
bxr_gamepad_held(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return gamepads_[index].buttons[button].is_down;
}

bool
bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return gamepads_[index].just_pressed_acc[button];
}

bool
bxr_gamepad_just_released(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return false;
  }

  return gamepads_[index].just_released_acc[button];
}

Uint64
bxr_gamepad_held_time(int index, bxr_gamepad_button_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(button >= 0 && button < BXR_GAMEPAD_BUTTON_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0;
  }

  if (gamepads_[index].buttons[button].is_down) {
    return SDL_GetTicks() - gamepads_[index].buttons[button].pressed_at;
  } else {
    return 0;
  }
}

float
bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0.0f;
  }

  return gamepads_[index].axis[axis];
}

float
bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(index >= 0 && index < BXR_GAMEPAD_MAX);
  BXR_ASSERT(axis >= 0 && axis < BXR_GAMEPAD_AXIS_SIZE);

  if (!bxr_gamepad_connected(index)) {
    return 0.0f;
  }

  return gamepads_[index].axis_prev[axis];
}

bool
bxr_gamepad_rumble(int index,
                   Uint16 low_frequency,
                   Uint16 high_frequency,
                   Uint32 duration_ms)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (gamepads_[index].gamepad == NULL) {
    return false;
  }

  low_frequency  = SDL_clamp(low_frequency, 0, 100);
  high_frequency = SDL_clamp(high_frequency, 0, 100);

  // Convert from percentage (0-100) to hex value (0x0000-0xFFFF)
  // 0xFFFF is max rumble value (from SDL3 docs)
  int low_frequency_hex  = (0xFFFF / 100) * low_frequency;
  int high_frequency_hex = (0xFFFF / 100) * high_frequency;

  // Start rumble
  return SDL_RumbleGamepad(gamepads_[index].gamepad,
                           low_frequency_hex,
                           high_frequency_hex,
                           duration_ms);
}
