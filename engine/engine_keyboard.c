#include <SDL3/SDL.h>

#include "engine_defs.h"
#include "engine_keyboard.h"

typedef struct _engine_key_state_s
{
  bool is_down;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at[ENGINE_KEY_SIZE];
} _engine_key_state_t;

typedef struct _engine_keyboard_s
{
  _engine_key_state_t keys[ENGINE_KEY_SIZE];
  bool just_pressed_acc[ENGINE_KEY_SIZE];
  bool just_released_acc[ENGINE_KEY_SIZE];
} _engine_keyboard_t;

static Uint32 _initialized     = 0;
static _engine_keyboard_t _key = { 0 };

void
engine_keyboard_setup(void)
{
  SDL_assert(_initialized == 0);
  _initialized = ENGINE_INIT_COOKIE;
}

void
engine_keyboard_begin_frame(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  for (int i = 0; i < ENGINE_KEY_SIZE; ++i) {
    _key.just_pressed_acc[i]  = _key.keys[i].just_pressed;
    _key.just_released_acc[i] = _key.keys[i].just_released;

    _key.keys[i].just_pressed  = false;
    _key.keys[i].just_released = false;
  }
}

void
engine_keyboard_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _initialized = 0;
  _key         = (_engine_keyboard_t){ 0 };
}

void
engine_key_down(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (!_key.keys[key].is_down) {
    _key.keys[key].pressed_at[key] = SDL_GetTicks();
    _key.keys[key].just_pressed    = true;
  }

  _key.keys[key].is_down = true;
}

void
engine_key_up(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (_key.keys[key].is_down) {
    _key.keys[key].just_released = true;
  }

  _key.keys[key].pressed_at[key] = 0;
  _key.keys[key].is_down         = false;
}

const char *
engine_key_name(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  SDL_Scancode scancode = (SDL_Scancode)key;
  if (scancode == SDL_SCANCODE_UNKNOWN) {
    return "Unknown";
  }

  SDL_Keycode keycode
      = SDL_GetKeyFromScancode(scancode, SDL_GetModState(), true);

  return SDL_GetKeyName(keycode);
}

const char *
engine_key_name_ex(engine_key_e key, engine_keymode_e mod)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  SDL_Scancode scancode = (SDL_Scancode)key;
  if (scancode == SDL_SCANCODE_UNKNOWN) {
    return "Unknown";
  }

  SDL_Keycode keycode = SDL_GetKeyFromScancode(scancode, (SDL_Keymod)mod, true);

  return SDL_GetKeyName(keycode);
}

bool
engine_key_held(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);
  return _key.keys[key].is_down;
}

bool
engine_key_just_pressed(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  return _key.just_pressed_acc[key];
}

bool
engine_key_just_released(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  return _key.just_released_acc[key];
}

Uint64
engine_key_held_time(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (_key.keys[key].is_down) {
    return SDL_GetTicks() - _key.keys[key].pressed_at[key];
  } else {
    return 0;
  }
}

bool
engine_keymod_pressed(engine_keymode_e mod)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  return (SDL_GetModState() & (SDL_Keymod)mod) != 0;
}
