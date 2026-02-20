#include "engine_mouse.h"
#include "engine_defs.h"

typedef struct _engine_mouse_s
{
  engine_mouse_click_e buttons[ENGINE_MOUSE_SIZE];
  engine_mouse_click_e buttons_prev[ENGINE_MOUSE_SIZE];

  Uint64 buttons_pressed_at[ENGINE_MOUSE_SIZE];

  float x;
  float y;

  float x_prev;
  float y_prev;

  float wheel_x;
  float wheel_y;
} _engine_mouse_t;

static Uint32 _initialized    = 0;
static _engine_mouse_t _mouse = { 0 };
static engine_context_t *_context;

void
engine_mouse_setup(engine_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = ENGINE_INIT_COOKIE;
  _context     = context;
}

void
engine_mouse_update()
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  memcpy(_mouse.buttons_prev, _mouse.buttons, sizeof(_mouse.buttons_prev));

  _mouse.x_prev = _mouse.x;
  _mouse.y_prev = _mouse.y;
}

void
engine_mouse_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _initialized = 0;
  _context     = NULL;
  _mouse       = (_engine_mouse_t){ 0 };
}

void
engine_mouse_button_down(engine_mouse_e button, engine_mouse_click_e click)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  if (_mouse.buttons[button] == ENGINE_MOUSE_NONE) {
    _mouse.buttons_pressed_at[button] = SDL_GetTicks();
  }

  _mouse.buttons[button] = click;
}

void
engine_mouse_button_up(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  _mouse.buttons_pressed_at[button] = 0;
  _mouse.buttons[button]            = 0;
}

void
engine_mouse_motion(float x, float y)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _mouse.x = x;
  _mouse.y = y;
}

void
engine_mouse_wheel_motion(float x, float y)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _mouse.wheel_x = x;
  _mouse.wheel_y = y;
}

float
engine_mouse_x(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  return _mouse.x;
}

float
engine_mouse_y(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  return _mouse.y;
}

float
engine_mouse_prev_x(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  return _mouse.x_prev;
}

float
engine_mouse_prev_y(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  return _mouse.y_prev;
}

bool
engine_mouse_pressed(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button] != ENGINE_MOUSE_NONE;
}

bool
engine_mouse_released(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button] == ENGINE_MOUSE_NONE;
}

bool
engine_mouse_just_pressed(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button] != ENGINE_MOUSE_NONE
         && _mouse.buttons_prev[button] == ENGINE_MOUSE_NONE;
}

bool
engine_mouse_just_released(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button] == ENGINE_MOUSE_NONE
         && _mouse.buttons_prev[button] != ENGINE_MOUSE_NONE;
}

bool
engine_mouse_double_clicked(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button] == ENGINE_MOUSE_DOUBLE;
}

Uint64
engine_mouse_held_time(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  if (!_mouse.buttons[button]) {
    return false;
  }

  return SDL_GetTicks() - _mouse.buttons_pressed_at[button];
}

float
engine_mouse_wheel_motion_x(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  return _mouse.wheel_x;
}

float
engine_mouse_wheel_motion_y(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  return _mouse.wheel_y;
}

void
engine_mouse_hide(bool hide)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  if (hide) {
    SDL_ShowCursor();
  } else {
    SDL_HideCursor();
  }
}

bool
engine_mouse_is_hidden(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  return SDL_CursorVisible();
}
