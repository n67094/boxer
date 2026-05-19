#include "bxr_context.h"
#include "bxr_assert.h"
#include "bxr_log.h"
#include "bxr_math.h"
#include "bxr_mem.h"

static bxr_context_t *context_ = NULL;

bxr_context_t *
bxr_context_get(void)
{
  if (context_ == NULL) {
    BXR_NEW(context_);
    if (!context_) {
      BXR_LOG_ERROR("Failed to create context");
      return NULL;
    }

    SDL_zerop(context_);
  }

  return context_;
}

void
bxr_context_set_window_dimensions(int width, int height)
{
  BXR_ASSERT(context_);

  context_->config.width  = width;
  context_->config.height = height;

  SDL_SetWindowSize(context_->window, width, height);
}

bxr_vec2_t
bxr_context_get_window_dimensions(void)
{
  BXR_ASSERT(context_);

  return (bxr_vec2_t){ .x = context_->config.width,
                       .y = context_->config.height };
}

void
bxr_context_set_fullscreen(bool fullscreen)
{
  BXR_ASSERT(context_);

  context_->config.fullscreen = fullscreen;

  SDL_SetWindowFullscreen(context_->window, fullscreen);
}

bool
bxr_context_is_fullscreen(void)
{
  BXR_ASSERT(context_);

  return context_->config.fullscreen;
}

void
bxr_context_set_resizable(bool resizable)
{
  BXR_ASSERT(context_);

  context_->config.resizable = resizable;

  SDL_SetWindowResizable(context_->window, resizable);
}

bool
bxr_context_is_resizable(void)
{
  BXR_ASSERT(context_);

  return context_->config.resizable;
}
