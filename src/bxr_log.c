#include <stdio.h>

#include "bxr_defs.h"
#include "bxr_log.h"

static bool has_colors_ = false;

static const char *styles_[] = {
  BXR_TERM_COLOR_RED,
  BXR_TERM_COLOR_YELLOW,
  BXR_TERM_COLOR_CYAN,
  "", // no color for info
};

static const SDL_LogPriority sdl_priorities_[] = {
  SDL_LOG_PRIORITY_ERROR,
  SDL_LOG_PRIORITY_WARN,
  SDL_LOG_PRIORITY_DEBUG,
  SDL_LOG_PRIORITY_INFO,
};

void
bxr_log_setup(void)
{
  // Set the default log priority to debug
  bxr_log_set_priority(BXR_LOG_PRIORITY_DEBUG);
}

void
bxr_log_teardown(void)
{
  // No resources to clean up
}

void
bxr_log(bxr_log_priority_e priority, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  if (has_colors_) {
    const size_t idx = (size_t)priority;
    if (idx < BXR_LOG_PRIORITY_SIZE)
      fputs(styles_[idx], stderr);
  }

  SDL_LogMessageV(
      SDL_LOG_CATEGORY_APPLICATION, sdl_priorities_[priority], fmt, args);

  if (has_colors_)
    fputs(BXR_TERM_RESET, stderr);

  va_end(args);
}

void
bxr_log_set_priority(bxr_log_priority_e priority)
{
  if (priority < BXR_LOG_PRIORITY_SIZE) {
    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, sdl_priorities_[priority]);
  }
}

void
bxr_log_enable_colors(bool enabled)
{
  has_colors_ = enabled;
}
