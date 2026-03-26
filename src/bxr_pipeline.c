#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_mem.h"
#include "bxr_pipeline.h"
#include "bxr_resource.h"

typedef struct _bxr_pipeline_s
{
  SDL_GPUGraphicsPipeline *pipeline;
} _bxr_pipeline_t;

static bxr_resource_t *_pipeline_resource = NULL;
static _bxr_pipeline_t *_pipelines        = NULL;

static Uint32 _initialized     = 0;
static bxr_context_t *_context = NULL;

void
bxr_pipeline_setup(bxr_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _context = context;

  _initialized = BXR_INIT_COOKIE;

  _pipeline_resource = bxr_resource_make(BXR_RESOURCES_PIPELINE_MAX);
  BXR_CALLOC(_pipelines, BXR_RESOURCES_PIPELINE_MAX, sizeof(_bxr_pipeline_t));
}

void
bxr_pipeline_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  bxr_resource_destroy(_pipeline_resource);
  BXR_FREE(_pipelines);

  _initialized = 0;
  _context     = NULL;
}

bxr_pipeline_t
bxr_pipeline_attach(SDL_GPUGraphicsPipeline *pipeline)
{
  SDL_assert(pipeline);
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  int slot_index = bxr_resource_acquire_slot(_pipeline_resource);
  if (slot_index == _BXR_RESOURCE_INVALID_SLOT) {
    SDL_ReleaseGPUGraphicsPipeline(_context->gpu_device, pipeline);
    bxr_set_error(BXR_ERROR_PIPELINE_MAKE);
    return (bxr_pipeline_t){ .id = BXR_INVALID_ID };
  }

  _pipelines[slot_index] = (_bxr_pipeline_t){
    .pipeline = pipeline,
  };

  return (bxr_pipeline_t){ .id = bxr_resource_gen_id(_pipeline_resource,
                                                     slot_index) };
}

SDL_GPUGraphicsPipeline *
bxr_pipeline_get(bxr_pipeline_t pipeline)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  int slot_index = bxr_resource_id_to_slot(pipeline.id);
  return _pipelines[slot_index].pipeline;
}

void
bxr_pipeline_destroy(bxr_pipeline_t pipeline)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  int slot_index = bxr_resource_id_to_slot(pipeline.id);
  bxr_resource_release_slot(_pipeline_resource, slot_index);

  _bxr_pipeline_t inner_pipeline = _pipelines[slot_index];

  SDL_ReleaseGPUGraphicsPipeline(_context->gpu_device, inner_pipeline.pipeline);

  _pipelines[slot_index] = (_bxr_pipeline_t){
    .pipeline = NULL,
  };
}
