#include "bxr_context.h"
#include "bxr_assert.h"
#include "bxr_error.h"
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
      bxr_set_error("Failed to create context");
      return NULL;
    }

    SDL_zerop(context_);
  }

  return context_;
}

SDL_GPUDevice *
bxr_context_get_gpu_device(void)
{
  BXR_ASSERT(context_);

  return context_->gpu_device;
}

SDL_Window *
bxr_context_get_window(void)
{
  BXR_ASSERT(context_);

  return context_->window;
}

bool
bxr_context_set_window_dimensions(int width, int height)
{
  BXR_ASSERT(context_);

  context_->config.width  = width;
  context_->config.height = height;

  if (!SDL_SetWindowSize(context_->window, width, height)) {
    return bxr_set_error("Failed to set window dimensions to %dx%d (error: %s)",
                         width,
                         height,
                         SDL_GetError());
  }

  return true;
}

void
bxr_context_set_virtual_window_dimensions(int width, int height)
{
  BXR_ASSERT(context_);

  context_->config.virtual_width  = width;
  context_->config.virtual_height = height;
}

bxr_vec2_t
bxr_context_get_virtual_window_dimensions(void)
{
  BXR_ASSERT(context_);

  return (bxr_vec2_t){ .x = context_->config.virtual_width,
                       .y = context_->config.virtual_height };
}

bxr_vec2_t
bxr_context_get_window_dimensions(void)
{
  BXR_ASSERT(context_);

  return (bxr_vec2_t){ .x = context_->config.width,
                       .y = context_->config.height };
}

bool
bxr_context_set_fullscreen(bool fullscreen)
{
  BXR_ASSERT(context_);

  context_->config.fullscreen = fullscreen;

  if (!SDL_SetWindowFullscreen(context_->window, fullscreen)) {
    return bxr_set_error("Failed to set fullscreen (error: %s)",
                         SDL_GetError());
  }

  return true;
}

bool
bxr_context_is_fullscreen(void)
{
  BXR_ASSERT(context_);

  return SDL_GetWindowFlags(context_->window) & SDL_WINDOW_FULLSCREEN;
}

bool
bxr_context_set_resizable(bool resizable)
{
  BXR_ASSERT(context_);

  context_->config.resizable = resizable;

  if (!SDL_SetWindowResizable(context_->window, resizable)) {
    return bxr_set_error("Failed to set resizable (error: %s)", SDL_GetError());
  }

  return true;
}

bool
bxr_context_is_resizable(void)
{
  BXR_ASSERT(context_);

  return context_->config.resizable;
}

Uint64
bxr_context_get_frame_count(void)
{
  BXR_ASSERT(context_);

  return context_->frame_count;
}
