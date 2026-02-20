#include <SDL3/SDL.h>

#include "engine_defs.h"
#include "engine_keyboard.h"

typedef struct _engine_keyboard_s
{
  bool keys[ENGINE_KEY_SIZE];
  bool keys_prev[ENGINE_KEY_SIZE];

  Uint64 keys_pressed_at[ENGINE_KEY_SIZE];
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
engine_keyboard_update(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  SDL_memcpy(_key.keys_prev, _key.keys, sizeof(_key.keys_prev));
}

void
engine_keyboard_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _initialized = 0;
  _key         = (_engine_keyboard_t){ 0 };
}

void
engine_key_up(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (_key.keys[key]) {
    _key.keys_pressed_at[key] = 0;
  }

  _key.keys[key] = false;
}

void
engine_key_down(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (!_key.keys[key]) {
    _key.keys_pressed_at[key] = SDL_GetTicks();
  }

  _key.keys[key] = true;
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
engine_key_pressed(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);
  return _key.keys[key];
}

bool
engine_key_released(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);
  return _key.keys[key];
}

bool
engine_key_just_pressed(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);
  return _key.keys[key] && !_key.keys_prev[key];
}

bool
engine_key_just_released(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);
  return _key.keys[key] && !_key.keys_prev[key];
}

Uint64
engine_key_held_time(engine_key_e key)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(key >= 0 && key < ENGINE_KEY_SIZE);

  if (_key.keys[key]) {
    return SDL_GetTicks() - _key.keys_pressed_at[key];
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
