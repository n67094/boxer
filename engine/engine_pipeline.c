#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_mem.h"
#include "engine_pipeline.h"
#include "engine_pool.h"
#include "engine_shader.h"

typedef struct _engine_pipeline_s
{
  SDL_GPUGraphicsPipeline *pipeline;
} _engine_pipeline_t;

static engine_pool_t *_pipeline_pool  = NULL;
static _engine_pipeline_t *_pipelines = NULL;

static Uint32 _initialized        = 0;
static engine_context_t *_context = NULL;

static SDL_GPUColorTargetBlendState
_engine_pipeline_blend_state(engine_blendmode_e blend_mode)
{
  SDL_GPUColorTargetBlendState blend;

  SDL_memset(&blend, 0, sizeof(SDL_GPUColorTargetBlendState));

  switch (blend_mode) {
  case ENGINE_BLENDMODE_BLEND:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  default: // ENGINE_BLENDMODE_NONE
    blend.enable_blend          = false;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  }

  return blend;
}

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
engine_pipeline_make(engine_shader_t shader,
                     engine_primitive_e primitive_type,
                     engine_blendmode_e blend_mode)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  SDL_GPUVertexInputState vertex_input_state
      = {.num_vertex_buffers = 1,
         .vertex_buffer_descriptions
         = (SDL_GPUVertexBufferDescription
                []){ { .slot               = 0,
                       .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                       .instance_step_rate = 0,
                       .pitch              = sizeof(engine_vertex_t) }, },
         .num_vertex_attributes = 2,
         .vertex_attributes
         = (SDL_GPUVertexAttribute
                []){ { .buffer_slot = 0,
                       .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                       .location    = 0,
                       .offset      = ENGINE_OFFSET_OF(engine_vertex_t, position) },
                     { .buffer_slot = 0,
                       .format      = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
                       .location    = 1,
                       .offset      = ENGINE_OFFSET_OF(engine_vertex_t, color) }, } };

  SDL_GPUColorTargetBlendState blend_state
      = _engine_pipeline_blend_state(blend_mode);

  SDL_GPUGraphicsPipelineTargetInfo target_info
      = { .num_color_targets         = 1,
          .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
              { .format = SDL_GetGPUSwapchainTextureFormat(_context->device,
                                                           _context->window),
                .blend_state = blend_state } } };

  SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(
      _context->device,
      &(SDL_GPUGraphicsPipelineCreateInfo){
          .target_info        = target_info,
          .primitive_type     = engine_sdl_primitive_type(primitive_type),
          .vertex_shader      = engine_shader_get_vertex(shader),
          .fragment_shader    = engine_shader_get_fragment(shader),
          .vertex_input_state = vertex_input_state,
      });

  if (pipeline == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create graphics pipeline (error: %s)",
                 SDL_GetError());

    engine_set_error(ENGINE_ERROR_PIPELINE_MAKE);
    return (engine_pipeline_t){ .id = ENGINE_INVALID_ID };
  }

  int slot_index = engine_pool_acquire_slot(_pipeline_pool);
  if (slot_index == _ENGINE_INVALID_SLOT) {
    SDL_ReleaseGPUGraphicsPipeline(_context->device, pipeline);
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

  SDL_ReleaseGPUGraphicsPipeline(_context->device, inner_pipeline.pipeline);

  _pipelines[slot_index] = (_engine_pipeline_t){
    .pipeline = NULL,
  };
}
