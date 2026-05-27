/**
 * File `bxr_log.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Logging utilities
 *
 * Basic usage:
 *
 *   BXR_LOG_INFO("Hello, %s!", "world");
 *   BXR_LOG_ERROR("Something went wrong: %d", error_code);
 *
 * Optionally enable ANSI terminal colors:
 *
 *   bxr_log_enable_colors(true);
 *
 */

#ifndef BXR_LOG_H_
#define BXR_LOG_H_

#include <SDL3/SDL.h>

typedef enum
{
  BXR_LOG_PRIORITY_ERROR = 0,
  BXR_LOG_PRIORITY_WARN,
  BXR_LOG_PRIORITY_DEBUG,
  BXR_LOG_PRIORITY_INFO,
  BXR_LOG_PRIORITY_SIZE
} bxr_log_priority_e;

/**
 * Internal API
 */

void bxr_log_setup(void);

void bxr_log_teardown(void);

/**
 * Public API
 */

/**
 * Log a message with the given priority and format string.
 *
 * `priority` is the log priority level of the message.
 *
 * `fmt` is the format string, similar to printf, followed by its arguments.
 */
void bxr_log(bxr_log_priority_e priority, const char *fmt, ...);

/**
 * Set the minimum log priority level. Messages with a priority lower than this
 * will not be logged.
 *
 * `priority` is the minimum log priority level to set.
 */
void bxr_log_set_priority(bxr_log_priority_e priority);

/**
 * Enable or disable ANSI terminal colors for log messages.
 *
 * `enabled` is true to enable colors, false to disable.
 */
void bxr_log_enable_colors(bool enabled);

/**
 * Convenience logging macros for different log levels.
 *
 * `...` is the format string and its arguments, similar to printf.
 */
#define BXR_LOG_ERROR(...) bxr_log(BXR_LOG_PRIORITY_ERROR, __VA_ARGS__)

/**
 * Convenience logging macros for different log levels.
 *
 * `...` is the format string and its arguments, similar to printf.
 */
#define BXR_LOG_WARN(...) bxr_log(BXR_LOG_PRIORITY_WARN, __VA_ARGS__)

/**
 * Convenience logging macros for different log levels.
 *
 * `...` is the format string and its arguments, similar to printf.
 */
#define BXR_LOG_DEBUG(...) bxr_log(BXR_LOG_PRIORITY_DEBUG, __VA_ARGS__)

/**
 * Convenience logging macros for different log levels.
 *
 * `...` is the format string and its arguments, similar to printf.
 */
#define BXR_LOG_INFO(...) bxr_log(BXR_LOG_PRIORITY_INFO, __VA_ARGS__)

#endif // BXR_LOG_H_
