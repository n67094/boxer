#include "bxr_error.h"
#include "bxr_assert.h"
#include "bxr_config.h"
#include "bxr_log.h"
#include "bxr_mem.h"

static SDL_TLSID tls_id_ = { 0 };

// Used when the per-thread buffer can't be allocated.
static char fallback_[BXR_ERROR_MAX_LENGTH] = { 0 };

static char *
bxr_error_buffer_(void)
{
  char *buffer = SDL_GetTLS(&tls_id_);
  if (buffer) {
    return buffer;
  }

  buffer = BXR_CALLOC(BXR_ERROR_MAX_LENGTH, 1);
  if (!buffer) {
    return fallback_;
  }

  if (!SDL_SetTLS(&tls_id_, buffer, SDL_free)) {
    BXR_FREE(buffer);
    return fallback_;
  }

  return buffer;
}

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

  SDL_strlcpy(bxr_error_buffer_(), msg, BXR_ERROR_MAX_LENGTH);

  BXR_LOG_DEBUG("Error: %s", msg);

  return false;
}

const char *
bxr_get_error(void)
{
  const char *buffer = SDL_GetTLS(&tls_id_);
  return buffer ? buffer : fallback_;
}

void
bxr_clear_error(void)
{
  char *buffer = SDL_GetTLS(&tls_id_);
  if (buffer) {
    buffer[0] = '\0';
  }
  fallback_[0] = '\0';
}
