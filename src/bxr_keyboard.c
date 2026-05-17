#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_defs.h"
#include "bxr_keyboard.h"

typedef struct bxr_key_state_s_
{
  bool is_down;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at[BXR_KEY_SIZE];
} bxr_key_state_t_;

typedef struct bxr_keyboard_s_
{
  bxr_key_state_t_ keys[BXR_KEY_SIZE];
  bool just_pressed_acc[BXR_KEY_SIZE];
  bool just_released_acc[BXR_KEY_SIZE];
} bxr_keyboard_t_;

static Uint32 initialized_  = 0;
static bxr_keyboard_t_ key_ = { 0 };

void
bxr_keyboard_setup(void)
{
  BXR_ASSERT(initialized_ == 0);
  initialized_ = BXR_INIT_COOKIE;
}

void
bxr_keyboard_begin_frame(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_KEY_SIZE; ++i) {
    key_.just_pressed_acc[i]  = key_.keys[i].just_pressed;
    key_.just_released_acc[i] = key_.keys[i].just_released;

    key_.keys[i].just_pressed  = false;
    key_.keys[i].just_released = false;
  }
}

void
bxr_keyboard_shutdown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  initialized_ = 0;
  key_         = (bxr_keyboard_t_){ 0 };
}

void
bxr_key_down(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  if (!key_.keys[key].is_down) {
    key_.keys[key].pressed_at[key] = SDL_GetTicks();
    key_.keys[key].just_pressed    = true;
  }

  key_.keys[key].is_down = true;
}

void
bxr_key_up(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  if (key_.keys[key].is_down) {
    key_.keys[key].just_released = true;
  }

  key_.keys[key].pressed_at[key] = 0;
  key_.keys[key].is_down         = false;
}

const char *
bxr_key_name(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  SDL_Scancode scancode = (SDL_Scancode)key;
  if (scancode == SDL_SCANCODE_UNKNOWN) {
    return "Unknown";
  }

  SDL_Keycode keycode
      = SDL_GetKeyFromScancode(scancode, SDL_GetModState(), true);

  return SDL_GetKeyName(keycode);
}

const char *
bxr_key_name_ex(bxr_key_e key, bxr_keymode_e mod)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  SDL_Scancode scancode = (SDL_Scancode)key;
  if (scancode == SDL_SCANCODE_UNKNOWN) {
    return "Unknown";
  }

  SDL_Keycode keycode = SDL_GetKeyFromScancode(scancode, (SDL_Keymod)mod, true);

  return SDL_GetKeyName(keycode);
}

bool
bxr_key_held(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);
  return key_.keys[key].is_down;
}

bool
bxr_key_just_pressed(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  return key_.just_pressed_acc[key];
}

bool
bxr_key_just_released(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  return key_.just_released_acc[key];
}

Uint64
bxr_key_held_time(bxr_key_e key)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(key >= 0 && key < BXR_KEY_SIZE);

  if (key_.keys[key].is_down) {
    return SDL_GetTicks() - key_.keys[key].pressed_at[key];
  } else {
    return 0;
  }
}

bool
bxr_keymod_pressed(bxr_keymode_e mod)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  return (SDL_GetModState() & (SDL_Keymod)mod) != 0;
}
