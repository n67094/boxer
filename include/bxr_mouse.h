/**
 * File `bxr_mouse.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Mouse Input Handling
 */

#ifndef BXR_MOUSE_H_
#define BXR_MOUSE_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"
#include "bxr_defs.h"

typedef enum
{
  BXR_MOUSE_NONE = 0,
  BXR_MOUSE_SINGLE,
  BXR_MOUSE_DOUBLE,
} bxr_mouse_click_e;

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

/**
 * Internal API
 */

/**
 * Initialize the mouse subsystem. Must be called before using any other mouse
 * functions.
 */
BXR_API void bxr_mouse_setup(void);

/**
 * Update the mouse state for the current frame. Must be called at the beginning
 * of each frame before processing input.
 */
BXR_API void bxr_mouse_begin_frame(void);

/**
 * Shutdown the mouse subsystem. Must be called when the application is shutting
 * down.
 */
BXR_API void bxr_mouse_shutdown(void);

/**
 * Handles a mouse button down event. Must be called when a mouse button event
 * is received from SDL.
 *
 * `button` is the mouse button that was pressed or released.
 *
 * `click` is the type of click (single or double).
 */
BXR_API void bxr_mouse_button_down(bxr_mouse_e button, bxr_mouse_click_e click);

/**
 * Handles a mouse button up event. Must be called when a mouse button up event
 * is received from SDL.
 *
 * `button` is the mouse button that was released.
 */
BXR_API void bxr_mouse_button_up(bxr_mouse_e button);

/**
 * Handles a mouse motion event. Must be called when a mouse motion event is
 * received from SDL.
 *
 * `x` is the new x position of the mouse cursor.
 *
 * `y` is the new y position of the mouse cursor.
 */
BXR_API void bxr_mouse_motion(float x, float y);

/**
 * Handles a mouse wheel motion event. Must be called when a mouse wheel motion
 * event is received from SDL.
 *
 * `x` is the amount of horizontal scroll.
 *
 * `y` is the amount of vertical scroll.
 */
BXR_API void bxr_mouse_wheel_motion(float x, float y);

/**
 * Public API
 */

/**
 * Get the current position of the mouse cursor.
 *
 * `return` the current position of the mouse cursor.
 */
BXR_API bxr_vec2_t bxr_mouse_pos(void);

/**
 * Get the position of the mouse cursor in the previous frame.
 *
 * `return` the position of the mouse cursor in the previous frame.
 */
BXR_API bxr_vec2_t bxr_mouse_prev_pos(void);

/**
 * Check if a mouse button is currently held down.
 *
 * `button` is the mouse button to check.
 *
 * `return` true if the button is currently held down, false otherwise.
 */
BXR_API bool bxr_mouse_held(bxr_mouse_e button);

/**
 * Check if a mouse button was just pressed this frame.
 *
 * `button` is the mouse button to check.
 *
 * `return` true if the button was just pressed this frame, false otherwise.
 */
BXR_API bool bxr_mouse_just_pressed(bxr_mouse_e button);

/**
 * Check if a mouse button was just released this frame.
 *
 * `button` is the mouse button to check.
 *
 * `return` true if the button was just released this frame, false otherwise.
 */
BXR_API bool bxr_mouse_just_released(bxr_mouse_e button);

/**
 * Check if a mouse button was double clicked this frame.
 *
 * `button` is the mouse button to check.
 *
 * `return` true if the button was double clicked this frame, false otherwise.
 */
BXR_API bool bxr_mouse_double_clicked(bxr_mouse_e button);

/**
 * Get the amount of time a mouse button has been held down.
 *
 * `button` is the mouse button to check.
 *
 * `return` the amount of time the button has been held down in milliseconds.
 */
BXR_API Uint64 bxr_mouse_held_time(bxr_mouse_e button);

/**
 * Get the amount of scroll from the mouse wheel in the current frame.
 *
 * `return` the amount of scroll from the mouse wheel in the current frame.
 */
BXR_API bxr_vec2_t bxr_mouse_wheel(void);

/**
 * Hide or show the mouse cursor.
 *
 * `hide` true to hide the mouse cursor, false to show it.
 */
BXR_API void bxr_mouse_hide(bool hide);

/**
 * Check if the mouse cursor is currently hidden.
 *
 * `return` true if the mouse cursor is currently hidden, false otherwise.
 */
BXR_API bool bxr_mouse_is_hidden(void);

#endif // BXR_MOUSE_H_
