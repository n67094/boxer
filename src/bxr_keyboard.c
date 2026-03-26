#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_keyboard.h"

typedef struct _bxr_key_state_s
{
  bool is_down;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at[BXR_KEY_SIZE];
} _bxr_key_state_t;

typedef struct _bxr_keyboard_s
{
  _bxr_key_state_t keys[BXR_KEY_SIZE];
  bool just_pressed_acc[BXR_KEY_SIZE];
  bool just_released_acc[BXR_KEY_SIZE];
} _bxr_keyboard_t;

static Uint32 _initialized  = 0;
static _bxr_keyboard_t _key = { 0 };

void
bxr_keyboard_setup(void)
{
  SDL_assert(_initialized == 0);
  _initialized = BXR_INIT_COOKIE;
}

void
bxr_keyboard_begin_frame(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_KEY_SIZE; ++i) {
    _key.just_pressed_acc[i]  = _key.keys[i].just_pressed;
    _key.just_released_acc[i] = _key.keys[i].just_released;

    _key.keys[i].just_pressed  = false;
    _key.keys[i].just_released = false;
  }
}

void
bxr_keyboard_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _initialized = 0;
  _key         = (_bxr_keyboard_t){ 0 };
}

void
bxr_key_down(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

  if (!_key.keys[key].is_down) {
    _key.keys[key].pressed_at[key] = SDL_GetTicks();
    _key.keys[key].just_pressed    = true;
  }

  _key.keys[key].is_down = true;
}

void
bxr_key_up(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

  if (_key.keys[key].is_down) {
    _key.keys[key].just_released = true;
  }

  _key.keys[key].pressed_at[key] = 0;
  _key.keys[key].is_down         = false;
}

const char *
bxr_key_name(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

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
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

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
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);
  return _key.keys[key].is_down;
}

bool
bxr_key_just_pressed(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

  return _key.just_pressed_acc[key];
}

bool
bxr_key_just_released(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

  return _key.just_released_acc[key];
}

Uint64
bxr_key_held_time(bxr_key_e key)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(key >= 0 && key < BXR_KEY_SIZE);

  if (_key.keys[key].is_down) {
    return SDL_GetTicks() - _key.keys[key].pressed_at[key];
  } else {
    return 0;
  }
}

bool
bxr_keymod_pressed(bxr_keymode_e mod)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  return (SDL_GetModState() & (SDL_Keymod)mod) != 0;
}
