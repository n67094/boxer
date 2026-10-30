/**
 * File `bxr_time.h`.
 *
 * # Time Utilities.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_TIME_H_
#define BXR_TIME_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

typedef SDL_TimerID bxr_timer_t;
typedef SDL_TimerCallback bxr_timer_cb_t;

/**
 * Get the time elapsed since SDL was initialized.
 *
 * `return` the number of milliseconds since SDL library initialization.
 */
BXR_INLINE Uint64
bxr_time_get_ticks(void)
{
  return SDL_GetTicks();
}

/**
 * Call a callback function after a delay, repeatedly if it asks to.
 *
 * Callback signature:
 *
 * ```c
 * Uint32 callback(void *udata, bxr_timer_t timer_id, Uint32 interval);
 * ```
 *
 * The callback returns the delay in milliseconds before its next run, or 0 to
 * stop the timer. It runs on a separate thread, so it must be thread-safe.
 *
 * `interval_ms` the delay before the first call, in milliseconds; also passed
 * to `callback` as `interval`.
 * `callback` the function to call when the delay has elapsed, must not be NULL.
 * `udata` a pointer passed to `callback`, may be NULL.
 *
 * `return` the timer ID on success, or 0 on failure.
 *
 * **Note**: remove the timer with `bxr_time_remove_timer()` if it does not
 * stop itself.
 */
BXR_INLINE bxr_timer_t
bxr_time_add_timer(Uint32 interval_ms, bxr_timer_cb_t callback, void *udata)
{
  return SDL_AddTimer(interval_ms, callback, udata);
}

/**
 * Remove a timer created with `bxr_time_add_timer()`.
 *
 * `timer_id` the ID of the timer to remove.
 */
BXR_INLINE bool
bxr_time_remove_timer(bxr_timer_t timer_id)
{
  return SDL_RemoveTimer(timer_id);
}

/**
 * Block the calling thread for at least the given duration.
 *
 * `ms` the number of milliseconds to wait.
 */
BXR_INLINE void
bxr_time_delay(Uint32 ms)
{
  SDL_Delay(ms);
}

#endif // BXR_TIME_H_
