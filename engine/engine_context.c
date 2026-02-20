#include "engine_context.h"
#include "engine_math.h"

static engine_context_t _context = { 0 };

engine_context_t *
engine_context_get(void)
{
  return &_context;
}

bool
engine_context_is_valid(void)
{
  return _context._initialized == ENGINE_INIT_COOKIE;
}

void
engine_context_set_window_size(int width, int height)
{
  engine_context_t *context = engine_context_get();
  context->config.width     = width;
  context->config.height    = height;

  SDL_SetWindowSize(context->window, width, height);
}

engine_vec2_t
engine_context_get_window_size(void)
{
  engine_context_t *context = engine_context_get();
  return (engine_vec2_t){ .x = context->config.width,
                          .y = context->config.height };
}

void
engine_context_set_fullscreen(bool fullscreen)
{
  engine_context_t *context  = engine_context_get();
  context->config.fullscreen = fullscreen;

  SDL_SetWindowFullscreen(context->window, fullscreen);
}

bool
engine_context_is_fullscreen(void)
{
  engine_context_t *context = engine_context_get();
  return context->config.fullscreen;
}

void
engine_context_set_resizable(bool resizable)
{
  engine_context_t *context = engine_context_get();
  context->config.resizable = resizable;

  SDL_SetWindowResizable(context->window, resizable);
}

bool
engine_context_is_resizable(void)
{
  engine_context_t *context = engine_context_get();
  return context->config.resizable;
}
