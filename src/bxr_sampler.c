#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include "bxr_context.h"
#include "bxr_error.h"
#include "bxr_mem.h"
#include "bxr_sampler.h"

static Uint32 _initialized     = 0;
static bxr_context_t *_context = NULL;

void
bxr_sampler_setup(void)
{
  SDL_assert(_initialized == 0);

  _initialized = BXR_INIT_COOKIE;
  _context     = bxr_context_get();
}

void
bxr_sampler_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _initialized = 0;
  _context     = NULL;
}

static SDL_GPUSamplerCreateInfo
_bxr_painter_sampler(bxr_sampler_e sampler_type)
{
  SDL_GPUSamplerCreateInfo sampler_info;

  BXR_MEMSET(&sampler_info, 0, sizeof(SDL_GPUSamplerCreateInfo));

  switch (sampler_type) {
  case BXR_SAMPLER_POINT_WRAP:
    sampler_info.min_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mag_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    break;
  case BXR_SAMPLER_LINEAR_CLAMP:
    sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    break;
  case BXR_SAMPLER_LINEAR_WRAP:
    sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    break;
  default: // BXR_SAMPLER_POINT_CLAMP
    sampler_info.min_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mag_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    break;
  }

  return sampler_info;
}

bxr_sampler_t
bxr_sampler_create(bxr_sampler_e sampler_type)
{
  bxr_sampler_t sampler;

  SDL_GPUSamplerCreateInfo sampler_info = _bxr_painter_sampler(sampler_type);

  sampler.sampler = SDL_CreateGPUSampler(_context->gpu_device, &sampler_info);

  if (!sampler.sampler) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create GPU sampler (error: %s)",
                 SDL_GetError());
    bxr_error_set(BXR_ERROR_SAMPLER_MAKE);
    return (bxr_sampler_t){ .sampler = NULL };
  }

  return sampler;
}

void
bxr_sampler_destroy(bxr_sampler_t *sampler)
{
  if (sampler && sampler->sampler) {
    SDL_ReleaseGPUSampler(_context->gpu_device, sampler->sampler);
    sampler->sampler = NULL;
  }
}
