#include "engine_mouse.h"
#include "engine_defs.h"

typedef struct _engine_mouse_button_state_s
{
  engine_mouse_click_e click;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at;
} _engine_mouse_button_state_t;

typedef struct _engine_mouse_s
{
  _engine_mouse_button_state_t buttons[ENGINE_MOUSE_SIZE];

  bool just_pressed_acc[ENGINE_MOUSE_SIZE];
  bool just_released_acc[ENGINE_MOUSE_SIZE];

  float x;
  float y;

  float x_prev;
  float y_prev;

  float wheel_x;
  float wheel_y;
} _engine_mouse_t;

static Uint32 _initialized    = 0;
static _engine_mouse_t _mouse = { 0 };

void
engine_mouse_setup(void)
{
  SDL_assert(_initialized == 0);

  _initialized = ENGINE_INIT_COOKIE;
}

void
engine_mouse_begin_frame()
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  for (int i = 0; i < ENGINE_MOUSE_SIZE; ++i) {
    _mouse.just_pressed_acc[i]  = _mouse.buttons[i].just_pressed;
    _mouse.just_released_acc[i] = _mouse.buttons[i].just_released;

    _mouse.buttons[i].just_pressed  = false;
    _mouse.buttons[i].just_released = false;
  }

  _mouse.wheel_x = 0;
  _mouse.wheel_y = 0;

  _mouse.x_prev = _mouse.x;
  _mouse.y_prev = _mouse.y;
}

void
engine_mouse_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _initialized = 0;
  _mouse       = (_engine_mouse_t){ 0 };
}

void
engine_mouse_button_down(engine_mouse_e button, engine_mouse_click_e click)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  if (_mouse.buttons[button].click == ENGINE_MOUSE_NONE) {
    _mouse.buttons[button].pressed_at   = SDL_GetTicks();
    _mouse.buttons[button].just_pressed = true;
  }

  _mouse.buttons[button].click = click;
}

void
engine_mouse_button_up(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  if (_mouse.buttons[button].click != ENGINE_MOUSE_NONE) {
    _mouse.buttons[button].just_released = true;
  }

  _mouse.buttons[button].pressed_at = 0;
  _mouse.buttons[button].click      = ENGINE_MOUSE_NONE;
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
engine_mouse_held(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button].click != ENGINE_MOUSE_NONE;
}

bool
engine_mouse_just_pressed(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.just_pressed_acc[button];
}

bool
engine_mouse_just_released(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.just_released_acc[button];
}

bool
engine_mouse_double_clicked(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  return _mouse.buttons[button].click == ENGINE_MOUSE_DOUBLE;
}

Uint64
engine_mouse_held_time(engine_mouse_e button)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(button > 0 && button < ENGINE_MOUSE_SIZE);

  if (!_mouse.buttons[button].click) {
    return false;
  }

  return SDL_GetTicks() - _mouse.buttons[button].pressed_at;
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
