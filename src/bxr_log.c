#include "bxr_log.h"
#include "bxr_assert.h"
#include "bxr_defs.h"

#define BXR_LOG_CATEGORY SDL_LOG_CATEGORY_CUSTOM

static bool has_colors_ = false;

static const char *styles_[] = {
  BXR_TERM_COLOR_CYAN,
  "", // no color for info
  BXR_TERM_COLOR_YELLOW,
  BXR_TERM_COLOR_RED,
};

static const SDL_LogPriority sdl_priorities_[] = {
  SDL_LOG_PRIORITY_DEBUG,
  SDL_LOG_PRIORITY_INFO,
  SDL_LOG_PRIORITY_WARN,
  SDL_LOG_PRIORITY_ERROR,
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
  BXR_ASSERT(priority < BXR_LOG_PRIORITY_SIZE);

  va_list args;
  va_start(args, fmt);

  if (!has_colors_) {
    SDL_LogMessageV(BXR_LOG_CATEGORY, sdl_priorities_[priority], fmt, args);
    va_end(args);
    return;
  }

  char *msg = NULL;
  if (SDL_vasprintf(&msg, fmt, args) >= 0) {
    SDL_LogMessage(BXR_LOG_CATEGORY,
                   sdl_priorities_[priority],
                   "%s%s%s",
                   styles_[priority],
                   msg,
                   BXR_TERM_RESET);
    SDL_free(msg);
  }

  va_end(args);
}

void
bxr_log_set_priority(bxr_log_priority_e priority)
{
  BXR_ASSERT(priority < BXR_LOG_PRIORITY_SIZE);

  if (priority < BXR_LOG_PRIORITY_SIZE) {
    SDL_SetLogPriority(BXR_LOG_CATEGORY, sdl_priorities_[priority]);
  }
}

void
bxr_log_enable_colors(bool enabled)
{
  has_colors_ = enabled;
}
