#include "bxr_context.h"
#include "bxr_math.h"

static bxr_context_t _context = { 0 };

bxr_context_t *
bxr_context_get(void)
{
  return &_context;
}

bool
bxr_context_is_valid(void)
{
  return _context._initialized == BXR_INIT_COOKIE;
}

void
bxr_context_set_window_dimensions(int width, int height)
{
  bxr_context_t *context = bxr_context_get();
  context->config.width  = width;
  context->config.height = height;

  SDL_SetWindowSize(context->window, width, height);
}

bxr_vec2_t
bxr_context_get_window_dimensions(void)
{
  bxr_context_t *context = bxr_context_get();
  return (bxr_vec2_t){ .x = context->config.width,
                       .y = context->config.height };
}

void
bxr_context_set_fullscreen(bool fullscreen)
{
  bxr_context_t *context     = bxr_context_get();
  context->config.fullscreen = fullscreen;

  SDL_SetWindowFullscreen(context->window, fullscreen);
}

bool
bxr_context_is_fullscreen(void)
{
  bxr_context_t *context = bxr_context_get();
  return context->config.fullscreen;
}

void
bxr_context_set_resizable(bool resizable)
{
  bxr_context_t *context    = bxr_context_get();
  context->config.resizable = resizable;

  SDL_SetWindowResizable(context->window, resizable);
}

bool
bxr_context_is_resizable(void)
{
  bxr_context_t *context = bxr_context_get();
  return context->config.resizable;
}
