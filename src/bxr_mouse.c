#include "bxr_mouse.h"
#include "bxr_defs.h"

typedef struct bxr_mouse_button_state_s_
{
  bxr_mouse_click_e click;
  bool just_pressed;
  bool just_released;
  Uint64 pressed_at;
} bxr_mouse_button_state_t_;

typedef struct bxr_mouse_s_
{
  bxr_mouse_button_state_t_ buttons[BXR_MOUSE_SIZE];

  bool just_pressed_acc[BXR_MOUSE_SIZE];
  bool just_released_acc[BXR_MOUSE_SIZE];

  float x;
  float y;

  float x_prev;
  float y_prev;

  float wheel_x;
  float wheel_y;
} bxr_mouse_t_;

static Uint32 initialized_ = 0;
static bxr_mouse_t_ mouse_ = { 0 };

void
bxr_mouse_setup(void)
{
  SDL_assert(initialized_ == 0);

  initialized_ = BXR_INIT_COOKIE;
}

void
bxr_mouse_begin_frame()
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_MOUSE_SIZE; ++i) {
    mouse_.just_pressed_acc[i]  = mouse_.buttons[i].just_pressed;
    mouse_.just_released_acc[i] = mouse_.buttons[i].just_released;

    mouse_.buttons[i].just_pressed  = false;
    mouse_.buttons[i].just_released = false;
  }

  mouse_.wheel_x = 0;
  mouse_.wheel_y = 0;

  mouse_.x_prev = mouse_.x;
  mouse_.y_prev = mouse_.y;
}

void
bxr_mouse_shutdown(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  initialized_ = 0;
  mouse_       = (bxr_mouse_t_){ 0 };
}

void
bxr_mouse_button_down(bxr_mouse_e button, bxr_mouse_click_e click)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  if (mouse_.buttons[button].click == BXR_MOUSE_NONE) {
    mouse_.buttons[button].pressed_at   = SDL_GetTicks();
    mouse_.buttons[button].just_pressed = true;
  }

  mouse_.buttons[button].click = click;
}

void
bxr_mouse_button_up(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  if (mouse_.buttons[button].click != BXR_MOUSE_NONE) {
    mouse_.buttons[button].just_released = true;
  }

  mouse_.buttons[button].pressed_at = 0;
  mouse_.buttons[button].click      = BXR_MOUSE_NONE;
}

void
bxr_mouse_motion(float x, float y)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  mouse_.x = x;
  mouse_.y = y;
}

void
bxr_mouse_wheel_motion(float x, float y)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  mouse_.wheel_x = x;
  mouse_.wheel_y = y;
}

bxr_vec2_t
bxr_mouse_pos(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  return bxr_vec2_create(mouse_.x, mouse_.y);
}

bxr_vec2_t
bxr_mouse_prev_pos(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  return bxr_vec2_create(mouse_.x_prev, mouse_.y_prev);
}

bool
bxr_mouse_held(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].click != BXR_MOUSE_NONE;
}

bool
bxr_mouse_just_pressed(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.just_pressed_acc[button];
}

bool
bxr_mouse_just_released(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.just_released_acc[button];
}

bool
bxr_mouse_double_clicked(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].click == BXR_MOUSE_DOUBLE;
}

Uint64
bxr_mouse_held_time(bxr_mouse_e button)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(button > 0 && button < BXR_MOUSE_SIZE);

  if (!mouse_.buttons[button].click) {
    return false;
  }

  return SDL_GetTicks() - mouse_.buttons[button].pressed_at;
}

bxr_vec2_t
bxr_mouse_wheel(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  return bxr_vec2_create(mouse_.wheel_x, mouse_.wheel_y);
}

void
bxr_mouse_hide(bool hide)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  if (hide) {
    SDL_ShowCursor();
  } else {
    SDL_HideCursor();
  }
}

bool
bxr_mouse_is_hidden(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  return SDL_CursorVisible();
}
