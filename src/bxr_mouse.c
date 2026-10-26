#include "bxr_mouse.h"
#include "bxr_assert.h"
#include "bxr_context.h"
#include "bxr_defs.h"

typedef struct bxr_mouse_button_state_s_
{
  Uint8 clicks;
  bool just_pressed;
  bool just_released;
  bool double_click;
  bool pending_just_pressed;
  bool pending_just_released;
  bool pending_double_click;
  Uint64 pressed_at;
} bxr_mouse_button_state_t_;

typedef struct bxr_mouse_s_
{
  bxr_mouse_button_state_t_ buttons[BXR_MOUSE_SIZE];

  // Written by events
  float x;
  float y;

  float wheel_x;
  float wheel_y;

  // Modified in bxr_mouse_begin_frame(), read by the public API
  float frame_x;
  float frame_y;

  float prev_x;
  float prev_y;

  float frame_wheel_x;
  float frame_wheel_y;
} bxr_mouse_t_;

static Uint32 initialized_ = 0;
static bxr_mouse_t_ mouse_ = { 0 };

static bxr_vec2_t
to_virtual_(float x, float y)
{
  bxr_vec2_t window_dim  = bxr_context_get_window_dimensions();
  bxr_vec2_t virtual_dim = bxr_context_get_virtual_window_dimensions();

  if (window_dim.x <= 0 || window_dim.y <= 0) {
    return bxr_vec2_create(0, 0);
  }

  return bxr_vec2_create(x * (virtual_dim.x / window_dim.x),
                         y * (virtual_dim.y / window_dim.y));
}

bool
bxr_mouse_setup(void)
{
  BXR_ASSERT(initialized_ == 0);

  // Start from the real cursor position to avoid a jump on the first frame
  SDL_GetMouseState(&mouse_.x, &mouse_.y);

  mouse_.frame_x = mouse_.x;
  mouse_.frame_y = mouse_.y;
  mouse_.prev_x  = mouse_.x;
  mouse_.prev_y  = mouse_.y;

  initialized_ = BXR_INIT_COOKIE;

  return true;
}

void
bxr_mouse_begin_frame()
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  for (int i = 0; i < BXR_MOUSE_SIZE; ++i) {
    mouse_.buttons[i].just_pressed  = mouse_.buttons[i].pending_just_pressed;
    mouse_.buttons[i].just_released = mouse_.buttons[i].pending_just_released;
    mouse_.buttons[i].double_click  = mouse_.buttons[i].pending_double_click;

    mouse_.buttons[i].pending_just_pressed  = false;
    mouse_.buttons[i].pending_just_released = false;
    mouse_.buttons[i].pending_double_click  = false;
  }

  mouse_.prev_x  = mouse_.frame_x;
  mouse_.prev_y  = mouse_.frame_y;
  mouse_.frame_x = mouse_.x;
  mouse_.frame_y = mouse_.y;

  mouse_.frame_wheel_x = mouse_.wheel_x;
  mouse_.frame_wheel_y = mouse_.wheel_y;
  mouse_.wheel_x       = 0;
  mouse_.wheel_y       = 0;
}

void
bxr_mouse_teardown(void)
{
  // Also called after a failed or skipped setup
  if (initialized_ != BXR_INIT_COOKIE) {
    return;
  }

  initialized_ = 0;
  mouse_       = (bxr_mouse_t_){ 0 };
}

void
bxr_mouse_button_down(bxr_mouse_e button, Uint8 clicks)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  if (mouse_.buttons[button].clicks == 0) {
    mouse_.buttons[button].pressed_at           = SDL_GetTicks();
    mouse_.buttons[button].pending_just_pressed = true;
  }

  if (clicks == 2) {
    mouse_.buttons[button].pending_double_click = true;
  }

  mouse_.buttons[button].clicks = clicks;
}

void
bxr_mouse_button_up(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  if (mouse_.buttons[button].clicks > 0) {
    mouse_.buttons[button].pending_just_released = true;
  }

  mouse_.buttons[button].pressed_at = 0;
  mouse_.buttons[button].clicks     = 0;
}

void
bxr_mouse_motion(float x, float y)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  mouse_.x = x;
  mouse_.y = y;
}

void
bxr_mouse_wheel_motion(float x, float y)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  mouse_.wheel_x += x;
  mouse_.wheel_y += y;
}

bxr_vec2_t
bxr_mouse_pos(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return to_virtual_(mouse_.frame_x, mouse_.frame_y);
}

bxr_vec2_t
bxr_mouse_window_pos(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  return bxr_vec2_create(mouse_.frame_x, mouse_.frame_y);
}

bxr_vec2_t
bxr_mouse_prev_pos(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return to_virtual_(mouse_.prev_x, mouse_.prev_y);
}

bxr_vec2_t
bxr_mouse_prev_window_pos(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  return bxr_vec2_create(mouse_.prev_x, mouse_.prev_y);
}

bool
bxr_mouse_held(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].clicks > 0;
}

bool
bxr_mouse_just_pressed(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].just_pressed;
}

bool
bxr_mouse_just_released(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].just_released;
}

Uint8
bxr_mouse_get_clicks(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].clicks;
}

bool
bxr_mouse_is_double_click(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  return mouse_.buttons[button].double_click;
}

Uint64
bxr_mouse_held_time(bxr_mouse_e button)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(button > 0 && button < BXR_MOUSE_SIZE);

  if (!mouse_.buttons[button].clicks) {
    return 0;
  }

  return SDL_GetTicks() - mouse_.buttons[button].pressed_at;
}

bxr_vec2_t
bxr_mouse_wheel(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  return bxr_vec2_create(mouse_.frame_wheel_x, mouse_.frame_wheel_y);
}

void
bxr_mouse_hide(bool hide)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  if (hide) {
    SDL_HideCursor();
  } else {
    SDL_ShowCursor();
  }
}

bool
bxr_mouse_is_visible(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  return SDL_CursorVisible();
}
