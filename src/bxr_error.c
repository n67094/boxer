#include "bxr_error.h"
#include "bxr_assert.h"
#include "bxr_config.h"
#include "bxr_log.h"

static char buffer_[BXR_ERROR_MAX_LENGTH] = { 0 };

bool
bxr_set_error(const char *fmt, ...)
{
  BXR_ASSERT(fmt);

  // Format into a temporary first, the previous message may be an argument.
  char msg[BXR_ERROR_MAX_LENGTH];

  va_list args;
  va_start(args, fmt);
  SDL_vsnprintf(msg, sizeof(msg), fmt, args);
  va_end(args);

  SDL_strlcpy(buffer_, msg, BXR_ERROR_MAX_LENGTH);

  BXR_LOG_DEBUG("Error: %s", msg);

  return false;
}

const char *
bxr_get_error(void)
{
  return buffer_;
}

void
bxr_clear_error(void)
{
  buffer_[0] = '\0';
}
