#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_mem.h"
#include "engine_pipeline.h"
#include "engine_pool.h"

typedef struct _engine_pipeline_s
{
  SDL_GPUGraphicsPipeline *pipeline;
} _engine_pipeline_t;

static engine_pool_t *_pipeline_pool  = NULL;
static _engine_pipeline_t *_pipelines = NULL;

static Uint32 _initialized        = 0;
static engine_context_t *_context = NULL;

void
engine_pipeline_setup(engine_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _context = context;

  _initialized = ENGINE_INIT_COOKIE;

  _pipeline_pool = engine_pool_make(ENGINE_RESOURCES_PIPELINE_MAX);
  ENGINE_CALLOC(
      _pipelines, ENGINE_RESOURCES_PIPELINE_MAX, sizeof(_engine_pipeline_t));
}

void
engine_pipeline_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  engine_pool_destroy(_pipeline_pool);
  ENGINE_FREE(_pipelines);

  _initialized = 0;
  _context     = NULL;
}

engine_pipeline_t
engine_pipeline_attach(SDL_GPUGraphicsPipeline *pipeline)
{
  SDL_assert(pipeline);
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_acquire_slot(_pipeline_pool);
  if (slot_index == _ENGINE_INVALID_SLOT) {
    SDL_ReleaseGPUGraphicsPipeline(_context->gpu_device, pipeline);
    engine_set_error(ENGINE_ERROR_PIPELINE_MAKE);
    return (engine_pipeline_t){ .id = ENGINE_INVALID_ID };
  }

  _pipelines[slot_index] = (_engine_pipeline_t){
    .pipeline = pipeline,
  };

  return (engine_pipeline_t){ .id = engine_pool_gen_id(_pipeline_pool,
                                                       slot_index) };
}

SDL_GPUGraphicsPipeline *
engine_pipeline_get(engine_pipeline_t pipeline)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_id_to_slot(pipeline.id);
  return _pipelines[slot_index].pipeline;
}

void
engine_pipeline_destroy(engine_pipeline_t pipeline)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_id_to_slot(pipeline.id);
  engine_pool_release_slot(_pipeline_pool, slot_index);

  _engine_pipeline_t inner_pipeline = _pipelines[slot_index];

  SDL_ReleaseGPUGraphicsPipeline(_context->gpu_device, inner_pipeline.pipeline);

  _pipelines[slot_index] = (_engine_pipeline_t){
    .pipeline = NULL,
  };
}
