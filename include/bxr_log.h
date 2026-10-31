/**
 * File `bxr_log.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Logging Utilities
 *
 * The default minimum priority is `BXR_LOG_PRIORITY_DEBUG`, so every level is
 * shown until `bxr_log_set_priority()` is called.
 *
 * Basic usage:
 *
 *   BXR_LOG_INFO("Hello, %s!", "world");
 *   BXR_LOG_ERROR("Something went wrong: %d", error_code);
 *
 * Optionally enable ANSI terminal colors:
 *
 *   bxr_log_enable_colors(true);
 */

#ifndef BXR_LOG_H_
#define BXR_LOG_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

void bxr_log_setup(void);
void bxr_log_teardown(void);

/**
 * ## Enums
 */

/**
 * Log priority levels.
 */
typedef enum
{
  BXR_LOG_PRIORITY_DEBUG = 0,
  BXR_LOG_PRIORITY_INFO,
  BXR_LOG_PRIORITY_WARN,
  BXR_LOG_PRIORITY_ERROR,
  BXR_LOG_PRIORITY_SIZE
} bxr_log_priority_e;

/**
 * ## API
 */

/**
 * Log a printf-style message at the given priority.
 *
 * `priority` level of the message, in `[0, BXR_LOG_PRIORITY_SIZE)`.
 *
 * `fmt` printf-style format string, followed by its arguments; must not be
 * NULL.
 */
BXR_API void bxr_log(bxr_log_priority_e priority, const char *fmt, ...);

/**
 * Set the minimum priority; less severe messages are dropped.
 *
 * Severity  order: DEBUG < INFO < WARN < ERROR.
 *
 * `priority` minimum level, in `[0, BXR_LOG_PRIORITY_SIZE)`.
 */
BXR_API void bxr_log_set_priority(bxr_log_priority_e priority);

/**
 * Enable or disable ANSI terminal colors for log messages.
 *
 * `enabled` true to enable colors, false to disable.
 */
BXR_API void bxr_log_enable_colors(bool enabled);

/**
 * Log a printf-style message at `BXR_LOG_PRIORITY_ERROR`.
 *
 * `...` printf-style format string followed by its arguments.
 */
#define BXR_LOG_ERROR(...) bxr_log(BXR_LOG_PRIORITY_ERROR, __VA_ARGS__)

/**
 * Log a printf-style message at `BXR_LOG_PRIORITY_WARN`.
 *
 * `...` printf-style format string followed by its arguments.
 */
#define BXR_LOG_WARN(...) bxr_log(BXR_LOG_PRIORITY_WARN, __VA_ARGS__)

/**
 * Log a printf-style message at `BXR_LOG_PRIORITY_DEBUG`.
 *
 * `...` printf-style format string followed by its arguments.
 */
#define BXR_LOG_DEBUG(...) bxr_log(BXR_LOG_PRIORITY_DEBUG, __VA_ARGS__)

/**
 * Log a printf-style message at `BXR_LOG_PRIORITY_INFO`.
 *
 * `...` printf-style format string followed by its arguments.
 */
#define BXR_LOG_INFO(...) bxr_log(BXR_LOG_PRIORITY_INFO, __VA_ARGS__)

#endif // BXR_LOG_H_
