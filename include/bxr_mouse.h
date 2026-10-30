/**
 * File `bxr_mouse.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Mouse Input Handling
 *
 * The mouse position is tracked in the game's virtual resolution, and the mouse
 * wheel scroll is tracked in the current frame.
 */

#ifndef BXR_MOUSE_H_
#define BXR_MOUSE_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_math.h"

/**
 * ## Enums
 */

/**
 * Mouse buttons.
 */
typedef enum
{
  BXR_MOUSE_UNKNOWN = 0,
  BXR_MOUSE_LEFT    = SDL_BUTTON_LEFT,
  BXR_MOUSE_MIDDLE  = SDL_BUTTON_MIDDLE,
  BXR_MOUSE_RIGHT   = SDL_BUTTON_RIGHT,
  BXR_MOUSE_X1      = SDL_BUTTON_X1,
  BXR_MOUSE_X2      = SDL_BUTTON_X2,
  BXR_MOUSE_SIZE    = 6,
} bxr_mouse_e;

bool bxr_mouse_setup(void);
void bxr_mouse_begin_frame(void);
void bxr_mouse_teardown(void);
void bxr_mouse_button_down(bxr_mouse_e button, Uint8 click);
void bxr_mouse_button_up(bxr_mouse_e button);
void bxr_mouse_motion(float x, float y);
void bxr_mouse_wheel_motion(float x, float y);

/**
 * ## API
 */

/**
 * Get the position of the mouse cursor in the game's virtual resolution.
 *
 * `return` the cursor position in virtual resolution coordinates.
 */
BXR_API bxr_vec2_t bxr_mouse_pos(void);

/**
 * Get the current position of the mouse cursor in window coordinates.
 *
 * `return` the cursor position in window coordinates.
 */
BXR_API bxr_vec2_t bxr_mouse_window_pos(void);

/**
 * Get the position of the mouse cursor in the previous frame in the game's
 * virtual resolution.
 *
 * `return` the cursor position in virtual resolution coordinates.
 */
BXR_API bxr_vec2_t bxr_mouse_prev_pos(void);

/**
 * Get the position of the mouse cursor in the previous frame in window
 * coordinates.
 *
 * `return` the cursor position in window coordinates.
 */
BXR_API bxr_vec2_t bxr_mouse_prev_window_pos(void);

/**
 * Check if a mouse button is currently held down.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` true if the button is held, false otherwise.
 */
BXR_API bool bxr_mouse_held(bxr_mouse_e button);

/**
 * Check if a mouse button was pressed during the previous frame.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` true if the button was just pressed, false otherwise.
 */
BXR_API bool bxr_mouse_just_pressed(bxr_mouse_e button);

/**
 * Check if a mouse button was released during the previous frame.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` true if the button was just released, false otherwise.
 */
BXR_API bool bxr_mouse_just_released(bxr_mouse_e button);

/**
 * Get the number of clicks for a mouse button.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` the number of clicks for the button.
 */
BXR_API Uint8 bxr_mouse_get_clicks(bxr_mouse_e button);

/**
 * Check if a mouse button was double clicked during the previous frame.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` true if the button was double clicked, false otherwise.
 */
BXR_API bool bxr_mouse_is_double_click(bxr_mouse_e button);

/**
 * Get how long a mouse button has been held down.
 *
 * `button` button to check, in `[BXR_MOUSE_LEFT, BXR_MOUSE_X2]`.
 *
 * `return` the time since the button was pressed in milliseconds, or 0 if it is
 * not held.
 */
BXR_API Uint64 bxr_mouse_held_time(bxr_mouse_e button);

/**
 * Get the mouse wheel scroll for the current frame.
 *
 * `return` the scroll amount in the x and y directions.
 */
BXR_API bxr_vec2_t bxr_mouse_wheel(void);

/**
 * Hide or show the mouse cursor.
 *
 * `hide` true to hide the cursor, false to show it.
 */
BXR_API void bxr_mouse_hide(bool hide);

/**
 * Check if the mouse cursor is currently visible.
 *
 * `return` true if the cursor is visible, false otherwise.
 */
BXR_API bool bxr_mouse_is_visible(void);

#endif // BXR_MOUSE_H_
