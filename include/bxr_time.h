#ifndef BXR_TIME_H_
#define BXR_TIME_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

typedef SDL_TimerID bxr_timer_t;
typedef SDL_TimerCallback bxr_timer_cb_t;

/**
 * `return` the current value of the timer counter in milliseconds.
 */
BXR_INLINE uint64_t
bxr_time_get_ticks()
{
  return SDL_GetTicks();
}

/**
 * Call a callback function at a future time.
 *
 * Callback signature:
 *
 * return the new callback time interval, or 0 to disable further runs of the
 * callback.
 *
 * ```c
 * uint32_t callback(void *udata, bxr_timer_t timer_id, uint32_t interval);
 * ```
 *
 * `interval_ms` the timer delay, in milliseconds, passed to `callback`.
 * `callback` the bxr_timer_cb_t function to call when the specified `interval`
 * `udata` a pointer that is passed to `callback`.
 */
BXR_INLINE bxr_timer_t
bxr_time_add_timer(uint32_t interval_ms, bxr_timer_cb_t callback, void *udata)
{
  return SDL_AddTimer(interval_ms, callback, udata);
}

/**
 * Remove a timer created with bxr_time_add_timer().
 *
 * `timer_id` the ID of the timer to remove.
 */
BXR_INLINE void
bxr_time_remove_timer(bxr_timer_t timer_id)
{
  SDL_RemoveTimer(timer_id);
}

/**
 * Wait a specified number of milliseconds before returning.
 *
 * `ms` the number of milliseconds to delay.
 */
BXR_INLINE void
bxr_time_delay(uint32_t ms)
{
  SDL_Delay(ms);
}

#endif // BXR_TIME_H_
