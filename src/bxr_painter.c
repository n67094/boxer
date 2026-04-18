#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_image.h"
#include "bxr_math.h"
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_shader.h"

typedef enum
{
  BXR_PAINTEr_COMMAND_NONE = 0,
  BXR_PAINTER_COMMAND_DRAW,
  BXR_PAINTER_COMMAND_VIEWPORT,
  BXR_PAINTER_COMMAND_SCISSOR
} _bxr_painter_draw_command_e;

typedef struct _bxr_painter_mat2x3_s
{
  float m00, m01, m02;
  float m10, m11, m12;
} _bxr_painter_mat2x3_t;

#define bxr_painter_mat2x3_identity()                                          \
  ((const _bxr_painter_mat2x3_t){ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f })

#define bxr_painter_mat2x3_make(m00, m01, m02, m10, m11, m12)                  \
  ((const _bxr_painter_mat2x3_t){ m00, m01, m02, m10, m11, m12 })

typedef enum
{
  BXR_PAINTER_UNIFORM_SLOT_VS = 0,
  BXR_PAINTER_UNIFORM_SLOT_FS = 1
} _bxr_painter_uniform_slot_e;

typedef union _bxr_painter_uniform_data_u
{
  float floats[BXR_PAINTER_CONTENT_SLOTS_MAX];
  uint8_t bytes[BXR_PAINTER_CONTENT_SLOTS_MAX * sizeof(float)];
} _bxr_painter_uniform_data_t;

typedef struct _bxr_painter_uniform_s
{
  Uint16 vs_size;
  Uint16 fs_size;
  _bxr_painter_uniform_data_t data;
} _bxr_painter_uniform_t;

typedef struct _bxr_painter_texture_uniform_s
{
  Uint32 count;
  bxr_image_t images[BXR_PAINTER_TEXTURE_SLOTS_MAX];
  SDL_GPUSampler *samplers[BXR_PAINTER_TEXTURE_SLOTS_MAX];
} _bxr_painter_texture_uniform_t;

typedef struct _bxr_painter_draw_args_s
{
  bxr_pipeline_t pipeline;
  bxr_region_t region;

  _bxr_painter_texture_uniform_t texture;

  Uint32 uniform_index;
  Uint32 vertex_index;
  Uint32 vertices_count;
} _bxr_painter_draw_args_t;

typedef union _bxr_painter_command_args_u
{
  _bxr_painter_draw_args_t draw;
  bxr_rect_t viewport;
  bxr_rect_t scissor;
} _bxr_painter_command_args_t;

typedef struct _bxr_painter_command_s
{
  _bxr_painter_draw_command_e cmd;
  _bxr_painter_command_args_t args;
} _bxr_painter_command_t;

typedef struct _bxr_painter_state_s
{
  _bxr_painter_mat2x3_t projection;
  _bxr_painter_mat2x3_t transform;
  _bxr_painter_mat2x3_t mvp;

  _bxr_painter_texture_uniform_t texture;
  _bxr_painter_uniform_t uniform;

  bxr_pipeline_t pipeline;

  bxr_blendmode_e blend_mode;

  bxr_vec2_t frame_dimension;
  bxr_rect_t viewport;
  bxr_rect_t scissor;
  bxr_color_t color;
  float thickness;

  Uint32 base_uniform;
  Uint32 base_vertex;
  Uint32 base_command;
} _bxr_painter_state_t;

typedef struct _bxr_painter_s
{
  SDL_GPUTransferBuffer *vertex_transfer_buffer;
  SDL_GPUBuffer *vertex_data_buffer;

  bxr_shader_t shader;

  bxr_pipeline_t pipelines[BXR_PRIMITIVE_SIZE * BXR_BLENDMODE_SIZE];

  SDL_GPUSampler *samplers[BXR_SAMPLER_SIZE];

  bxr_image_t white_image;

  // States stack
  Uint32 current_state;
  _bxr_painter_state_t
      states[BXR_PAINTER_STATE_MAX]; // save states for push/pop
  _bxr_painter_state_t state;        // current state

  // Transforms stack
  Uint32 current_transform;
  _bxr_painter_mat2x3_t
      transforms[BXR_PAINTER_TRANSFORMS_MAX]; // save transforms for push/pop

  // Vertecies stack
  Uint32 current_vertex;
  bxr_vertex_t
      vertices[BXR_PAINTER_VERTICES_MAX]; // save vertecies for draw calls

  // Uniforms stack
  Uint32 current_uniform;
  _bxr_painter_uniform_t
      uniforms[BXR_PAINTER_COMMANDS_MAX]; // save uniforms for draw calls

  // Commands management
  Uint32 current_command;
  _bxr_painter_command_t
      commands[BXR_PAINTER_COMMANDS_MAX]; // save draw commands for flush
} _bxr_painter_t;

static _bxr_painter_t _painter = { 0 };
static Uint32 _initialized     = 0;
static bxr_context_t *_context = NULL;

static SDL_GPUColorTargetBlendState
_bxr_painter_pipeline_blend_state(bxr_blendmode_e blend_mode)
{
  SDL_GPUColorTargetBlendState blend;

  SDL_memset(&blend, 0, sizeof(SDL_GPUColorTargetBlendState));

  switch (blend_mode) {
  case BXR_BLENDMODE_BLEND:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  case BXR_BLENDMODE_BLEND_PREMULTIPLIED:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  case BXR_BLENDMODE_ADD:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  case BXR_BLENDMODE_ADD_PREMULTIPLIED:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  case BXR_BLENDMODE_MOD:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  case BXR_BLENDMODE_MUL:
    blend.enable_blend          = true;
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    break;
  default: // BXR_BLENDMODE_NONE
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

bxr_pipeline_t
bxr_painter_pipeline_make(bxr_shader_t shader,
                          bxr_primitive_e primitive_type,
                          bxr_blendmode_e blend_mode)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_GPUVertexInputState vertex_input_state =
  {
    .num_vertex_buffers = 1,
    .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription []){
      {
        .slot               = 0,
        .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
        .pitch              = sizeof(bxr_vertex_t)
      },
    },
    .num_vertex_attributes = 2,
    .vertex_attributes = (SDL_GPUVertexAttribute []){
      {
        .buffer_slot = 0,
        .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        .location    = 0,
        .offset      = BXR_OFFSET_OF(bxr_vertex_t,position)
      },
      {
        .buffer_slot = 0,
        .format      = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
        .location    = 1,
        .offset      = BXR_OFFSET_OF(bxr_vertex_t, color)
      }
    }
  };

  SDL_GPUColorTargetBlendState blend_state
      = _bxr_painter_pipeline_blend_state(blend_mode);

  SDL_GPUGraphicsPipelineTargetInfo target_info
      = { .num_color_targets         = 1,
          .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
              { .format = SDL_GetGPUSwapchainTextureFormat(_context->gpu_device,
                                                           _context->window),
                .blend_state = blend_state } } };

  SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(
      _context->gpu_device,
      &(SDL_GPUGraphicsPipelineCreateInfo){
          .target_info        = target_info,
          .primitive_type     = (SDL_GPUPrimitiveType)primitive_type,
          .vertex_shader      = bxr_shader_get_vertex(shader),
          .fragment_shader    = bxr_shader_get_fragment(shader),
          .vertex_input_state = vertex_input_state,
      });

  if (pipeline == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create graphics pipeline (error: %s)",
                 SDL_GetError());

    bxr_set_error(BXR_ERROR_PIPELINE_MAKE);
    return (bxr_pipeline_t){ .id = BXR_INVALID_ID };
  }

  return bxr_pipeline_attach(pipeline);
}

static bxr_pipeline_t
_bxr_painter_lookup_pipeline(bxr_primitive_e primitive_type,
                             bxr_blendmode_e blend_mode)
{
  bxr_pipeline_t pipeline
      = _painter.pipelines[primitive_type * BXR_BLENDMODE_SIZE + blend_mode];

  if (pipeline.id == BXR_INVALID_ID) {
    pipeline = bxr_painter_pipeline_make(
        _painter.shader, primitive_type, blend_mode);
    _painter.pipelines[primitive_type * BXR_BLENDMODE_SIZE + blend_mode]
        = pipeline;
  }

  return pipeline;
}

static SDL_GPUSamplerCreateInfo
_bxr_painter_sampler(bxr_sampler_e sampler_type)
{
  SDL_GPUSamplerCreateInfo sampler_info;

  SDL_memset(&sampler_info, 0, sizeof(SDL_GPUSamplerCreateInfo));

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

BXR_INLINE _bxr_painter_uniform_t *
_bxr_painter_next_uniform(void)
{
  if (_painter.current_uniform < BXR_PAINTER_COMMANDS_MAX) {
    return &_painter.uniforms[_painter.current_uniform++];
  } else {
    bxr_set_error(BXR_ERROR_PAINTER_UNIFORMS_FULL);
    return NULL;
  }
}

BXR_INLINE _bxr_painter_uniform_t *
_bxr_painter_prev_uniform(void)
{
  if (_painter.current_uniform > 0) {
    return &_painter.uniforms[_painter.current_uniform - 1];
  } else {
    return NULL;
  }
}

BXR_INLINE bxr_vertex_t *
_bxr_painter_next_vertices(Uint32 count)
{
  if (_painter.current_vertex + count <= BXR_PAINTER_VERTICES_MAX) {
    bxr_vertex_t *vertices = &_painter.vertices[_painter.current_vertex];
    _painter.current_vertex += count;

    return vertices;
  } else {
    bxr_set_error(BXR_ERROR_PAINTER_VERTICES_FULL);
    return NULL;
  }
}

BXR_INLINE _bxr_painter_command_t *
_bxr_painter_next_command(void)
{
  if ((_painter.current_command < BXR_PAINTER_COMMANDS_MAX)) {
    return &_painter.commands[_painter.current_command++];
  } else {
    bxr_set_error(BXR_ERROR_PAINTER_COMMANDS_FULL);
    return NULL;
  }
}

BXR_INLINE _bxr_painter_command_t *
_bxr_painter_prev_command(Uint32 count)
{
  if (_painter.current_command - _painter.state.base_command >= count) {
    return &_painter.commands[_painter.current_command - count];
  } else {
    return NULL;
  }
}

BXR_INLINE _bxr_painter_mat2x3_t
_bxr_painter_default_projection(int width, int height)
{
  float w = (float)width;
  float h = (float)height;

  return bxr_painter_mat2x3_make(2.0f / w, 0.0f, -1.0f, 0.0f, -2.0f / h, 1.0f);
}

BXR_INLINE _bxr_painter_mat2x3_t
_bxr_painter_mul_projection_transform(_bxr_painter_mat2x3_t *projection,
                                      _bxr_painter_mat2x3_t *transform)
{
  float x = projection->m00;
  float y = projection->m11;

  _bxr_painter_mat2x3_t out = { 0 };

  out.m00 = x * transform->m00;
  out.m01 = x * transform->m01;
  out.m02 = x * transform->m02 + projection->m02;

  out.m10 = y * transform->m10;
  out.m11 = y * transform->m11;
  out.m12 = y * transform->m12 + projection->m12;

  return out;
}

BXR_INLINE bxr_vec2_t
_bxr_painter_mat3_mul_vec2(_bxr_painter_mat2x3_t *m, const bxr_vec2_t *v)
{
  return bxr_vec2_make(m->m00 * v->x + m->m01 * v->y + m->m02,
                       m->m10 * v->x + m->m11 * v->y + m->m12);
}

BXR_INLINE void
_bxr_painter_transform(_bxr_painter_mat2x3_t *matrix,
                       bxr_vec2_t *dst,
                       const bxr_vec2_t *src,
                       Uint32 count)
{
  for (Uint32 i = 0; i < count; ++i) {
    dst[i] = _bxr_painter_mat3_mul_vec2(matrix, &src[i]);
  }
}

void
bxr_painter_setup(bxr_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = BXR_INIT_COOKIE;
  _context     = context;

  _painter.white_image = bxr_image_load_mem(
      2, 2, (Uint32[]){ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF });

  if (_painter.white_image.id == BXR_INVALID_ID) {
    // Error is handled in bxr_image_mem
    return;
  }

  // Create buffers for vertex data
  _painter.vertex_transfer_buffer = SDL_CreateGPUTransferBuffer(
      _context->gpu_device,
      &(SDL_GPUTransferBufferCreateInfo){
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size  = BXR_PAINTER_VERTICES_MAX * sizeof(bxr_vertex_t),

      });

  if (_painter.vertex_transfer_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create vertex transfer buffer (error: %s)",
                 SDL_GetError());
    bxr_set_error(BXR_ERROR_PAINTER_SETUP);
    return;
  }

  _painter.vertex_data_buffer = SDL_CreateGPUBuffer(
      _context->gpu_device,
      &(SDL_GPUBufferCreateInfo){
          .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
          .size  = BXR_PAINTER_VERTICES_MAX * sizeof(bxr_vertex_t),
      });
  if (_painter.vertex_data_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create vertex data buffer (error: %s)",
                 SDL_GetError());
    bxr_set_error(BXR_ERROR_PAINTER_SETUP);
    return;
  }

  _painter.shader = bxr_shader_make(
      &(bxr_shader_desc_t){ .name                 = "data/shaders/painter.vert",
                            .num_samplers         = 0,
                            .num_uniform_buffers  = 0,
                            .num_storage_buffers  = 0,
                            .num_storage_textures = 0 },
      &(bxr_shader_desc_t){ .name         = "data/shaders/painter.frag",
                            .num_samplers = BXR_PAINTER_TEXTURE_SLOTS_MAX,
                            .num_uniform_buffers  = 0,
                            .num_storage_buffers  = 0,
                            .num_storage_textures = 0 });

  bool pips_ok = true;
  pips_ok
      &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_POINTS, BXR_BLENDMODE_NONE)
             .id
         != BXR_INVALID_ID;
  pips_ok
      &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_POINTS, BXR_BLENDMODE_BLEND)
             .id
         != BXR_INVALID_ID;
  pips_ok
      &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_LINES, BXR_BLENDMODE_NONE)
             .id
         != BXR_INVALID_ID;
  pips_ok
      &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_LINES, BXR_BLENDMODE_BLEND)
             .id
         != BXR_INVALID_ID;
  pips_ok &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_LINE_STRIP,
                                          BXR_BLENDMODE_NONE)
                 .id
             != BXR_INVALID_ID;
  pips_ok &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_LINE_STRIP,
                                          BXR_BLENDMODE_BLEND)
                 .id
             != BXR_INVALID_ID;
  pips_ok &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_TRIANGLES,
                                          BXR_BLENDMODE_NONE)
                 .id
             != BXR_INVALID_ID;
  pips_ok &= _bxr_painter_lookup_pipeline(BXR_PRIMITIVE_TRIANGLES,
                                          BXR_BLENDMODE_BLEND)
                 .id
             != BXR_INVALID_ID;

  if (!pips_ok) {
    bxr_painter_shutdown();
    bxr_set_error(BXR_ERROR_PAINTER_SETUP);
    return;
  }

  for (int i = 0; i < BXR_SAMPLER_SIZE; ++i) {
    SDL_GPUSamplerCreateInfo sampler_info
        = _bxr_painter_sampler((bxr_sampler_e)i);
    SDL_GPUSampler *sampler
        = SDL_CreateGPUSampler(_context->gpu_device, &sampler_info);

    if (sampler == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Failed to create sampler (error: %s)",
                   SDL_GetError());
      bxr_set_error(BXR_ERROR_PAINTER_SETUP);
      return;
    }

    _painter.samplers[i] = sampler;
  }
}

void
bxr_painter_shutdown()
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_ReleaseGPUTransferBuffer(_context->gpu_device,
                               _painter.vertex_transfer_buffer);
  SDL_ReleaseGPUBuffer(_context->gpu_device, _painter.vertex_data_buffer);

  for (int i = 0; i < BXR_PRIMITIVE_SIZE * BXR_BLENDMODE_SIZE; ++i) {
    if (_painter.pipelines[i].id != BXR_INVALID_ID) {
      bxr_pipeline_destroy(_painter.pipelines[i]);
      _painter.pipelines[i] = (bxr_pipeline_t){ .id = BXR_INVALID_ID };
    }
  }

  bxr_shader_destroy(_painter.shader);

  for (int i = 0; i < BXR_SAMPLER_SIZE; ++i) {
    SDL_ReleaseGPUSampler(_context->gpu_device, _painter.samplers[i]);
  }

  bxr_image_destroy(_painter.white_image);

  _initialized = 0;
  _context     = NULL;
}

void
bxr_painter_begin(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  bxr_vec2_t dimension = bxr_context_get_window_dimensions();

  float width  = dimension.x;
  float height = dimension.y;

  // Save current state
  _painter.states[_painter.current_state++] = _painter.state;

  _painter.state.projection = _bxr_painter_default_projection(width, height);
  _painter.state.transform  = bxr_painter_mat2x3_identity();
  _painter.state.mvp        = _painter.state.projection;

  _painter.state.texture.count     = 1;
  _painter.state.texture.images[0] = _painter.white_image;
  _painter.state.texture.samplers[0]
      = _painter.samplers[BXR_SAMPLER_POINT_CLAMP];

  bxr_image_t image = { .id = BXR_INVALID_ID };
  for (int i = 1; i < BXR_PAINTER_TEXTURE_SLOTS_MAX; ++i) {
    _painter.state.texture.images[i] = image;
    _painter.state.texture.samplers[i]
        = _painter.samplers[BXR_SAMPLER_POINT_CLAMP];
  }

  memset(&_painter.state.uniform, 0, sizeof(_bxr_painter_uniform_t));
  _painter.state.uniform
      = (_bxr_painter_uniform_t){ .vs_size = 0, .fs_size = 0 };

  _painter.state.blend_mode = BXR_BLENDMODE_NONE;

  _painter.state.frame_dimension = bxr_vec2_make(width, height);
  _painter.state.viewport        = (bxr_rect_t){ 0, 0, width, height };
  _painter.state.scissor         = (bxr_rect_t){ 0, 0, -1, -1 };
  _painter.state.color           = BXR_COLOR_WHITE;

  _painter.state.thickness    = SDL_max(1.0f / width, 1.0f / height);
  _painter.state.base_vertex  = _painter.current_vertex;
  _painter.state.base_uniform = _painter.current_uniform;
  _painter.state.base_command = _painter.current_command;
}

void
bxr_painter_flush()
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  Uint32 end_command = _painter.current_command;
  Uint32 end_vertex  = _painter.current_vertex;

  Uint32 vertex_count
      = end_vertex - _painter.state.base_vertex; // Number of vertices to draw

  // Rewind indexes
  _painter.current_vertex  = _painter.state.base_vertex;
  _painter.current_uniform = _painter.state.base_uniform;
  _painter.current_command = _painter.state.base_command;

  // Nothing to be drawn
  if (end_command <= _painter.state.base_command) {
    return;
  }

  // Upload vertices
  bxr_vertex_t *vertex_data = (bxr_vertex_t *)SDL_MapGPUTransferBuffer(
      _context->gpu_device, _painter.vertex_transfer_buffer, true);
  if (vertex_data == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to map vertex transfer buffer (error: %s)",
                 SDL_GetError());

    bxr_set_error(BXR_ERROR_PAINTER_FLUSH);
    return;
  }

  SDL_memcpy(vertex_data,
             &_painter.vertices[_painter.state.base_vertex],
             vertex_count * sizeof(bxr_vertex_t));

  SDL_UnmapGPUTransferBuffer(_context->gpu_device,
                             _painter.vertex_transfer_buffer);

  // Copy pass
  SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(_context->cmd_buffer);

  SDL_UploadToGPUBuffer(
      copy_pass,
      &(SDL_GPUTransferBufferLocation){
          .transfer_buffer = _painter.vertex_transfer_buffer, .offset = 0 },
      &(SDL_GPUBufferRegion){
          .buffer = _painter.vertex_data_buffer,
          .offset = _painter.state.base_vertex * sizeof(bxr_vertex_t),
          .size   = (Uint32)((vertex_count) * sizeof(bxr_vertex_t)) },
      true);

  SDL_EndGPUCopyPass(copy_pass);

  // Render pass
  SDL_GPURenderPass *render_pass
      = SDL_BeginGPURenderPass(_context->cmd_buffer,
                               &(SDL_GPUColorTargetInfo){
                                   .texture     = _context->target_texture,
                                   .clear_color = { 0, 0, 0, 1 },
                                   .load_op     = SDL_GPU_LOADOP_CLEAR,
                                   .store_op    = SDL_GPU_STOREOP_STORE,
                                   .cycle       = false,
                               },
                               1,
                               NULL);

  // Flush commands

  Uint32 cur_pipeline_id   = BXR_IMPOSSIBLE_ID;
  Uint32 cur_uniform_index = BXR_IMPOSSIBLE_ID;
  Uint32 cur_image_ids[BXR_PAINTER_TEXTURE_SLOTS_MAX];
  for (int i = 0; i < BXR_PAINTER_TEXTURE_SLOTS_MAX; ++i) {
    cur_image_ids[i] = BXR_IMPOSSIBLE_ID;
  }

  for (Uint32 i = _painter.state.base_command; i < end_command; ++i) {
    _bxr_painter_command_t *cmd = &_painter.commands[i];

    // Forward declartion
    SDL_Rect scissor         = { 0 };
    SDL_GPUViewport viewport = { 0 };

    switch (cmd->cmd) {
    case BXR_PAINTER_COMMAND_DRAW: {
      bool rebind_uniforms = false;
      bool rebind_texture  = false;

      // Check if pipeline needs to be changed and bind it
      if (cmd->args.draw.pipeline.id != cur_pipeline_id) {
        cur_pipeline_id = cmd->args.draw.pipeline.id;

        // Bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass,
                                    bxr_pipeline_get(cmd->args.draw.pipeline));

        // When pipeline changes we need to rebind uniforms and textures
        rebind_uniforms = true;
        rebind_texture  = true;
      }

      SDL_GPUTextureSamplerBinding
          image_bindings[BXR_PAINTER_TEXTURE_SLOTS_MAX];

      for (int j = 0; j < BXR_PAINTER_TEXTURE_SLOTS_MAX; ++j) {
        Uint32 image_id = BXR_INVALID_ID;

        if (j < cmd->args.draw.texture.count) {
          image_id = cmd->args.draw.texture.images[j].id;
        }

        if (image_id != cur_image_ids[j]) {
          cur_image_ids[j] = image_id;

          if (image_id != BXR_INVALID_ID) {
            image_bindings[j] = (SDL_GPUTextureSamplerBinding){
              .texture
              = bxr_image_get_texture(cmd->args.draw.texture.images[j]),
              .sampler = cmd->args.draw.texture.samplers[j]
            };
          } else {
            image_bindings[j] = (SDL_GPUTextureSamplerBinding){
              .texture = bxr_image_get_texture(_painter.white_image),
              .sampler = _painter.samplers[BXR_SAMPLER_POINT_CLAMP]
            };
          }

          // When image changes we need to rebind textures
          rebind_texture = true;
        }
      }

      if (rebind_texture) {
        SDL_BindGPUFragmentSamplers(
            render_pass, 0, image_bindings, BXR_PAINTER_TEXTURE_SLOTS_MAX);
      }

      // Rebind uniforms if needed
      if (rebind_uniforms && cur_uniform_index != BXR_IMPOSSIBLE_ID) {
        _bxr_painter_uniform_t *uniform
            = &_painter.uniforms[cmd->args.draw.uniform_index];

        if (uniform->vs_size > 0) {
          SDL_PushGPUVertexUniformData(_context->cmd_buffer,
                                       BXR_PAINTER_UNIFORM_SLOT_VS,
                                       &uniform->data.bytes[0],
                                       uniform->vs_size);
        }
        if (uniform->fs_size > 0) {
          SDL_PushGPUFragmentUniformData(_context->cmd_buffer,
                                         BXR_PAINTER_UNIFORM_SLOT_FS,
                                         &uniform->data.bytes[0],
                                         uniform->fs_size);
        }
      }

      // In every case we need to bind vertex buffers
      SDL_BindGPUVertexBuffers(
          render_pass,
          0,
          &(SDL_GPUBufferBinding){ .buffer = _painter.vertex_data_buffer,
                                   .offset = cmd->args.draw.vertex_index
                                             * sizeof(bxr_vertex_t) },
          1);

      SDL_DrawGPUPrimitives(
          render_pass, cmd->args.draw.vertices_count, 1, 0, 0);
      break;
    }
    case BXR_PAINTER_COMMAND_VIEWPORT:
      viewport = (SDL_GPUViewport){
        .x = cmd->args.viewport.x,
        .y = cmd->args.viewport.y,
        .w = cmd->args.viewport.w,
        .h = cmd->args.viewport.h,
      };
      SDL_SetGPUViewport(render_pass, &viewport);
      break;
    case BXR_PAINTER_COMMAND_SCISSOR:
      scissor = (SDL_Rect){
        .x = (int)cmd->args.scissor.x,
        .y = (int)cmd->args.scissor.y,
        .w = (int)cmd->args.scissor.w,
        .h = (int)cmd->args.scissor.h,
      };
      SDL_SetGPUScissor(render_pass, &scissor);
      break;
    default:
      break;
    }
  }

  SDL_EndGPURenderPass(render_pass);
}

void
bxr_painter_end(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _painter.state = _painter.states[--_painter.current_state];
}

bxr_vec2_t
bxr_painter_get_frame_dimension(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  return _painter.state.frame_dimension;
}

void
bxr_painter_set_projection(float left, float right, float bottom, float top)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  float width  = right - left;
  float height = top - bottom;

  _painter.state.projection = bxr_painter_mat2x3_make(2.0f / width,
                                                      0.0f,
                                                      -(right + left) / width,
                                                      0.0f,
                                                      2.0f / height,
                                                      -(top + bottom) / height);

  _painter.state.mvp = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_reset_projection(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.projection = _bxr_painter_default_projection(
      (int)_painter.state.viewport.w, (int)_painter.state.viewport.h);

  _painter.state.mvp = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_push_transform(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);
  SDL_assert(_painter.current_transform < BXR_PAINTER_TRANSFORMS_MAX);

  _painter.transforms[_painter.current_transform++] = _painter.state.transform;
}

void
bxr_painter_pop_transform(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);
  SDL_assert(_painter.current_transform > 0);

  _painter.state.transform = _painter.transforms[--_painter.current_transform];
  _painter.state.mvp       = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_reset_transform(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.transform = bxr_painter_mat2x3_identity();
  _painter.state.mvp       = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_translate(float x, float y)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // multiply by translate matrix:
  // 1.0f, 0.0f, tx,
  // 0.0f, 1.0f, ty,

  _painter.state.transform.m02
      += x * _painter.state.transform.m00 + y * _painter.state.transform.m01;
  _painter.state.transform.m12
      += x * _painter.state.transform.m10 + y * _painter.state.transform.m11;

  _painter.state.mvp = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_rotate(float angle)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  float c = bxr_cos(angle);
  float s = bxr_sin(angle);

  // Multiply by rotation matrix:
  //   c,   -s, 0.0f,
  //   s,    c, 0.0f,

  _bxr_painter_mat2x3_t rotation = bxr_painter_mat2x3_make(
      c * _painter.state.transform.m00 + s * _painter.state.transform.m01,
      -s * _painter.state.transform.m00 + c * _painter.state.transform.m01,
      _painter.state.transform.m02,
      c * _painter.state.transform.m10 + s * _painter.state.transform.m11,
      -s * _painter.state.transform.m10 + c * _painter.state.transform.m11,
      _painter.state.transform.m12);

  _painter.state.transform = rotation;
  _painter.state.mvp       = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_rotate_at(float angle, float ax, float ay)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_translate(ax, ay);
  bxr_painter_rotate(angle);
  bxr_painter_translate(-ax, -ay);
}

void
bxr_painter_scale(float sx, float sy)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // Multiply by scale matrix:
  //   sx, 0.0f, 0.0f,
  // 0.0f,   sy, 0.0f,

  _painter.state.transform.m00 *= sx;
  _painter.state.transform.m01 *= sy;
  _painter.state.transform.m10 *= sx;
  _painter.state.transform.m11 *= sy;

  _painter.state.mvp = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_scale_at(float sx, float sy, float ax, float ay)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_translate(ax, ay);
  bxr_painter_scale(sx, sy);
  bxr_painter_translate(-ax, -ay);
}

void
bxr_painter_set_pipeline(bxr_pipeline_t pipeline)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _painter.state.pipeline = pipeline;

  // Reset uniforms when pipeline changes
  SDL_memset(&_painter.state.uniform, 0, sizeof(_bxr_painter_uniform_t));
}

void
bxr_painter_reset_pipeline(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  bxr_pipeline_t pipeline = { .id = BXR_INVALID_ID };

  bxr_painter_set_pipeline(pipeline);
}

void
bxr_painter_set_uniform(const void *vs_data,
                        size_t vs_size,
                        const void *fs_data,
                        size_t fs_size)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.state.pipeline.id != BXR_INVALID_ID);

  size_t size = vs_size + fs_size;

  SDL_assert(size <= BXR_PAINTER_CONTENT_SLOTS_MAX * sizeof(float));

  if (vs_size > 0) {
    SDL_assert(vs_data != NULL);
    SDL_memcpy(&_painter.state.uniform.data.bytes[0], vs_data, vs_size);
  }
  if (fs_size > 0) {
    SDL_assert(fs_data != NULL);
    SDL_memcpy(&_painter.state.uniform.data.bytes[vs_size], fs_data, fs_size);
  }

  size_t old_size
      = _painter.state.uniform.vs_size + _painter.state.uniform.fs_size;

  if (size != old_size) {
    // Zero out the rest of the uniform data
    SDL_memset((Uint8 *)(&_painter.state.uniform) + size, 0, old_size - size);
  }

  _painter.state.uniform.vs_size = (Uint16)vs_size;
  _painter.state.uniform.fs_size = (Uint16)fs_size;
}

void
bxr_painter_reset_uniform(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.state.pipeline.id != BXR_INVALID_ID);

  bxr_painter_set_uniform(NULL, 0, NULL, 0);
}

void
bxr_painter_set_blend_mode(bxr_blendmode_e blend_mode)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.blend_mode = blend_mode;
}

void
bxr_painter_reset_blend_mode(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.blend_mode = BXR_BLENDMODE_BLEND;
}

void
bxr_painter_set_color(bxr_color_t color)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.color = color;
}

bxr_color_t
bxr_painter_get_color(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  return _painter.state.color;
}

void
bxr_painter_reset_color(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.color = BXR_COLOR_WHITE;
}

void
bxr_painter_set_image(int channel, bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  if (_painter.state.texture.images[channel].id == image.id) {
    return;
  }

  _painter.state.texture.images[channel] = image;

  // Recalculate texture count
  int texture_count = _painter.state.texture.count;
  for (int i = SDL_max(channel, texture_count - 1); i >= 0; --i) {
    if (_painter.state.texture.images[i].id != BXR_INVALID_ID) {
      texture_count = i + 1;
      break;
    }
  }

  _painter.state.texture.count = texture_count;
}

bxr_image_t
bxr_painter_get_image(int channel)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  return _painter.state.texture.images[channel];
}

void
bxr_painter_unset_image(int channel)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_set_image(channel, (bxr_image_t){ .id = BXR_INVALID_ID });
}

void
bxr_painter_reset_image(int channel)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_set_image(channel, _painter.white_image);
}

void
bxr_painter_set_sampler(int channel, bxr_sampler_e sampler)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  _painter.state.texture.samplers[channel] = _painter.samplers[sampler];
}

void
bxr_painter_reset_sampler(int channel)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(channel >= 0 && channel < BXR_PAINTER_TEXTURE_SLOTS_MAX);
  SDL_assert(_painter.current_state > 0);

  _painter.state.texture.samplers[channel]
      = _painter.samplers[BXR_SAMPLER_POINT_CLAMP];
}

void
bxr_painter_viewport(float x, float y, float w, float h)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // If no change in viewport, skip
  if (_painter.state.viewport.x == x && _painter.state.viewport.y == y
      && _painter.state.viewport.w == w && _painter.state.viewport.h == h) {
    return;
  }

  // Try to reuse previous command
  _bxr_painter_command_t *cmd = _bxr_painter_prev_command(1);
  if (cmd && cmd->cmd != BXR_PAINTER_COMMAND_VIEWPORT) {
    cmd = _bxr_painter_next_command();
  }
  if (!cmd) {
    return;
  }

  SDL_memset(&cmd->args.viewport, 0, sizeof(bxr_rect_t));

  bxr_rect_t viewport = bxr_rect_make(x, y, w, h);

  cmd->cmd           = BXR_PAINTER_COMMAND_VIEWPORT;
  cmd->args.viewport = viewport;

  // When viewport changes, scissor needs to be updated to keep the same region
  if (!(_painter.state.scissor.w < 0 && _painter.state.scissor.h < 0)) {
    _painter.state.scissor.x += x - _painter.state.viewport.x;
    _painter.state.scissor.y += y - _painter.state.viewport.y;
  }

  _painter.state.viewport   = viewport;
  _painter.state.thickness  = BXR_MAX(1.0f / w, 1.0f / h);
  _painter.state.projection = _bxr_painter_default_projection(w, h);
  _painter.state.mvp        = _bxr_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
bxr_painter_reset_viewport(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_viewport(0,
                       0,
                       (float)_painter.state.frame_dimension.x,
                       (float)_painter.state.frame_dimension.y);
}

void
bxr_painter_scissor(float x, float y, float w, float h)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // Skip if scissor is the same
  if (_painter.state.scissor.x == x && _painter.state.scissor.y == y
      && _painter.state.scissor.w == w && _painter.state.scissor.h == h) {
    return;
  }

  // Try to reuse previous command
  _bxr_painter_command_t *cmd = _bxr_painter_prev_command(1);
  if (cmd && cmd->cmd != BXR_PAINTER_COMMAND_SCISSOR) {
    cmd = _bxr_painter_next_command();
  }
  if (!cmd) {
    return;
  }

  // Coordinates scissor relative to viewport
  bxr_rect_t viewport_scissor = bxr_rect_make(
      _painter.state.viewport.x + x, _painter.state.viewport.y + y, w, h);

  // Reset scissor
  if (w < 0 && h < 0) {
    viewport_scissor.x = 0;
    viewport_scissor.y = 0;
    viewport_scissor.w = _painter.state.frame_dimension.x;
    viewport_scissor.h = _painter.state.frame_dimension.y;
  }

  SDL_memset(&cmd->args.scissor, 0, sizeof(bxr_rect_t));

  cmd->cmd          = BXR_PAINTER_COMMAND_SCISSOR;
  cmd->args.scissor = viewport_scissor;

  _painter.state.scissor = bxr_rect_make(x, y, w, h);
}

void
bxr_painter_reset_scissor(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.scissor = (bxr_rect_t){ 0, 0, -1, -1 };
}

void
bxr_painter_reset_state(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  bxr_painter_reset_viewport();
  bxr_painter_reset_scissor();
  bxr_painter_reset_projection();
  bxr_painter_reset_transform();
  bxr_painter_reset_blend_mode();
  bxr_painter_reset_color();
  bxr_painter_reset_uniform();
  bxr_painter_reset_pipeline();
}

static bool
_bxr_painter_merge_draw_commands(SDL_GPUGraphicsPipeline *pipeline,
                                 _bxr_painter_texture_uniform_t *texture,
                                 _bxr_painter_uniform_t *uniform,
                                 Uint32 vertex_index,
                                 Uint32 vertices_count,
                                 SDL_GPUPrimitiveType primitive_type)
{
#if BXR_PAINTER_OPTIMIZE_DEPTH > 0
  _bxr_command_t *prev_cmd = NULL;
  _bxr_command_t *inter_cmds[BXR_PAINTER_OPTIMIZE_DEPTH];
  Uint32 inter_count = 0;

  // Find a command that is mergeable
  Uint32 lookup_depht = BXR_PAINTER_OPTIMIZE_DEPTH;
  for (Uint32 depth = 1; depth <= lookup_depht; ++depth) {
    _bxr_command_t *cmd = _bxr_prev_command(depth + 1);

    // Stop on nonexistent command
    if (!cmd) {
      break;
    }

    // command was optimized awaw, continue looking
    if (cmd->cmd == BXR_COMMAND_NONE) {
      lookup_depht++;
      continue;
    }

    // Stop on scissor or viewport
    if (cmd->cmd != BXR_COMMAND_DRAW) {
      break;
    }

    // Only command with the same pipeline, bindings and (TODO uniforms) can be
    // merged
    // TODO
  }

#endif
  return false;
}

void
_bxr_painter_queue_draw(bxr_pipeline_t pipeline,
                        bxr_region_t region,
                        Uint32 vertex_index,
                        Uint32 vertices_count,
                        bxr_primitive_e primitive_type)
{
  /*
  if (region.x0 > 1.0f || region.y0 > 1.0f || region.x1 < 0.0f
      || region.y1 < 0.0f) {
    _cur_vertex -= vertices_count; // rollback allocated vertices
    return;
  }
  */

  /* TODO
  // Try to merge with previous draw command
  if (primitive_type != SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP
      && primitive_type != SDL_GPU_PRIMITIVETYPE_LINESTRIP
      && _bxr_painter_merge_draw_commands(pipeline,
                                         _state.image,
                                         vertex_index,
                                         vertices_count,
                                         primitive_type)) {
    return;
  }
  */

  _bxr_painter_uniform_t *uniform = NULL;
  if (_painter.state.pipeline.id != BXR_INVALID_ID) {
    pipeline = _painter.state.pipeline;
    uniform  = &_painter.state.uniform;
  }

  Uint32 uniform_index = BXR_IMPOSSIBLE_ID;

  if (uniform) {
    _bxr_painter_uniform_t *prev_uniform = _bxr_painter_prev_uniform();

    bool reuse_uniform
        = prev_uniform
          && (SDL_memcmp(prev_uniform, uniform, sizeof(_bxr_painter_uniform_t))
              == 0);

    if (!reuse_uniform) {
      _bxr_painter_uniform_t *next_uniform = _bxr_painter_next_uniform();
      if (!next_uniform) {
        _painter.current_vertex
            -= vertices_count; // rollback allocated vertices
        return;
      }
      *next_uniform = _painter.state.uniform;
    }

    uniform_index
        = _painter.current_uniform - 1; // - 1 since _bxr_painter_next_uniform
                                        // already incremented the index
  }

  // New draw command
  _bxr_painter_command_t *cmd = _bxr_painter_next_command();

  if (!cmd) {
    _painter.current_vertex -= vertices_count; // rollback allocated vertices
    return;
  }

  cmd->cmd                = BXR_PAINTER_COMMAND_DRAW;
  cmd->args.draw.pipeline = pipeline;
  cmd->args.draw.texture  = _painter.state.texture;
  // TODO region for optimization
  cmd->args.draw.uniform_index  = uniform_index;
  cmd->args.draw.vertex_index   = vertex_index;
  cmd->args.draw.vertices_count = vertices_count;
}

static void
_bxr_painter_draw_solid(bxr_primitive_e primitive_type,
                        const bxr_vec2_t *vertices,
                        Uint32 vertices_count)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (vertices_count == 0) {
    return;
  }

  // Setup vertices
  Uint32 vertex_index = _painter.current_vertex;
  bxr_vertex_t *v     = _bxr_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  float thickness           = (primitive_type == BXR_PRIMITIVE_POINTS
                               || primitive_type == BXR_PRIMITIVE_LINES
                               || primitive_type == BXR_PRIMITIVE_LINE_STRIP)
                                  ? _painter.state.thickness
                                  : 1.0f;
  bxr_color_t color         = _painter.state.color;
  _bxr_painter_mat2x3_t mvp = _painter.state.mvp;
  bxr_region_t region       = { FLOAT_MAX, FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

  for (Uint32 i = 0; i < vertices_count; ++i) {
    bxr_vec2_t p = _bxr_painter_mat3_mul_vec2(&mvp, &vertices[i]);

    region.x0 = BXR_MIN(region.x0, p.x - thickness);
    region.y0 = BXR_MIN(region.y0, p.y - thickness);
    region.x1 = BXR_MAX(region.x1, p.x + thickness);
    region.y1 = BXR_MAX(region.y1, p.y + thickness);

    v[i].position = p;
    v[i].texcoord = bxr_vec2_make(0.0f, 0.0f);
    v[i].color    = color;
  }

  bxr_pipeline_t pipeline
      = _bxr_painter_lookup_pipeline(primitive_type, _painter.state.blend_mode);

  // Queue draw
  _bxr_painter_queue_draw(
      pipeline, region, vertex_index, vertices_count, primitive_type);
}

void
bxr_painter_clear(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // Setup vertices
  Uint32 vertices_count = 6;
  Uint32 vertex_index   = _painter.current_vertex;

  bxr_vertex_t *v = _bxr_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  // Compute vertices
  const bxr_vec2_t quad[4] = {
    { -1.0f, -1.0f }, // bottom-left
    { 1.0f, -1.0f },  // bottom-right
    { 1.0f, 1.0f },   // top-right
    { -1.0f, 1.0f }   // top-left
  };

  const bxr_vec2_t texcoord = { 0.0f, 0.0f };
  bxr_color_t color         = _painter.state.color;

  v[0] = (bxr_vertex_t){ .position = quad[0],
                         .texcoord = texcoord,
                         .color    = color };
  v[1] = (bxr_vertex_t){ .position = quad[1],
                         .texcoord = texcoord,
                         .color    = color };
  v[2] = (bxr_vertex_t){ .position = quad[2],
                         .texcoord = texcoord,
                         .color    = color };
  v[3] = (bxr_vertex_t){ .position = quad[2],
                         .texcoord = texcoord,
                         .color    = color };
  v[4] = (bxr_vertex_t){ .position = quad[3],
                         .texcoord = texcoord,
                         .color    = color };
  v[5] = (bxr_vertex_t){ .position = quad[0],
                         .texcoord = texcoord,
                         .color    = color };

  bxr_region_t region = { -1.0f, -1.0f, 1.0f, 1.0f };

  bxr_pipeline_t pipeline = _bxr_painter_lookup_pipeline(
      BXR_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);

  _bxr_painter_queue_draw(
      pipeline, region, vertex_index, vertices_count, BXR_PRIMITIVE_TRIANGLES);
}

void
bxr_painter_draw(bxr_primitive_e primitive_type,
                 const bxr_vertex_t *vertices,
                 Uint32 vertices_count)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (vertices_count == 0) {
    return;
  }

  // Setup vertices
  Uint32 vertex_index = _painter.current_vertex;
  bxr_vertex_t *v     = _bxr_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  float thickness           = (primitive_type == BXR_PRIMITIVE_POINTS
                               || primitive_type == BXR_PRIMITIVE_LINES
                               || primitive_type == BXR_PRIMITIVE_LINE_STRIP)
                                  ? _painter.state.thickness
                                  : 1.0f;
  _bxr_painter_mat2x3_t mvp = _painter.state.mvp;
  bxr_region_t region       = { FLOAT_MAX, FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

  for (Uint32 i = 0; i < vertices_count; ++i) {
    bxr_vec2_t p = _bxr_painter_mat3_mul_vec2(&mvp, &vertices[i].position);

    region.x0 = BXR_MIN(region.x0, p.x - thickness);
    region.y0 = BXR_MIN(region.y0, p.y - thickness);
    region.x1 = BXR_MAX(region.x1, p.x + thickness);
    region.y1 = BXR_MAX(region.y1, p.y + thickness);

    v[i].position = p;
    v[i].texcoord = vertices[i].texcoord;
    v[i].color    = vertices[i].color;
  }

  bxr_pipeline_t pipeline
      = _bxr_painter_lookup_pipeline(primitive_type, _painter.state.blend_mode);

  // Queue draw
  _bxr_painter_queue_draw(
      pipeline, region, vertex_index, vertices_count, primitive_type);
}

void
bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count)
{
  _bxr_painter_draw_solid(BXR_PRIMITIVE_POINTS, points, count);
}

void
bxr_painter_draw_point(bxr_vec2_t point)
{
  bxr_painter_draw_points(&point, 1);
}

void
bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count)
{
  _bxr_painter_draw_solid(
      BXR_PRIMITIVE_LINES, (const bxr_vec2_t *)lines, count * 2);
}

void
bxr_painter_draw_line(bxr_line_t line)
{
  bxr_painter_draw_lines(&line, 1);
}

void
bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count)
{
  _bxr_painter_draw_solid(
      BXR_PRIMITIVE_LINE_STRIP, (const bxr_vec2_t *)points, count);
}

void
bxr_painter_draw_triangles(const bxr_triangle_t *triangles, Uint32 count)
{
  _bxr_painter_draw_solid(
      BXR_PRIMITIVE_TRIANGLES, (const bxr_vec2_t *)triangles, count * 3);
}

void
bxr_painter_draw_triangle_filled(bxr_triangle_t triangle)
{
  bxr_painter_draw_triangles(&triangle, 1);
}

void
bxr_painter_draw_triangles_strip(const bxr_vec2_t *points, Uint32 count)
{
  _bxr_painter_draw_solid(
      BXR_PRIMITIVE_TRIANGLE_STRIP, (const bxr_vec2_t *)points, count);
}

void
bxr_painter_draw_rect_filled(bxr_rect_t rect)
{
  bxr_painter_draw_rects_filled(&rect, 1);
}

void
bxr_painter_draw_rects_filled(const bxr_rect_t *rects, Uint32 count)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (count == 0) {
    return;
  }

  // Setup vertices
  Uint32 total_vertices = count * 6; // 2 triangles per rect, 3 vertices each
  Uint32 vertex_index   = _painter.current_vertex;
  bxr_vertex_t *v       = _bxr_painter_next_vertices(total_vertices);
  if (!v) {
    return;
  }

  // Compute vertices
  const bxr_rect_t *rect    = rects;
  bxr_color_t color         = _painter.state.color;
  _bxr_painter_mat2x3_t mvp = _painter.state.mvp;
  bxr_region_t region       = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

  for (Uint32 i = 0; i < count; ++i, ++rect) {
    bxr_vec2_t quad[4] = {
      { rect->x, rect->y + rect->h },           // bottom-left
      { rect->x + rect->w, rect->y + rect->h }, // bottom-right
      { rect->x + rect->w, rect->y },           // top-right
      { rect->x, rect->y },                     // top-left
    };

    _bxr_painter_transform(&mvp, quad, quad, 4);

    for (Uint32 j = 0; j < 4; ++j) {
      region.x0 = SDL_min(region.x0, quad[j].x);
      region.y0 = SDL_min(region.y0, quad[j].y);
      region.x1 = SDL_max(region.x1, quad[j].x);
      region.y1 = SDL_max(region.y1, quad[j].y);
    }

    const bxr_vec2_t texcoords[4] = {
      { 0.0f, 1.0f }, // bottom-left
      { 1.0f, 1.0f }, // bottom-right
      { 1.0f, 0.0f }, // top-right
      { 0.0f, 0.0f }  // top-left
    };

    // Make two triangles to form the quad
    v[0] = (bxr_vertex_t){ .position = quad[0],
                           .texcoord = texcoords[0],
                           .color    = color };
    v[1] = (bxr_vertex_t){ .position = quad[1],
                           .texcoord = texcoords[1],
                           .color    = color };
    v[2] = (bxr_vertex_t){ .position = quad[2],
                           .texcoord = texcoords[2],
                           .color    = color };
    v[3] = (bxr_vertex_t){ .position = quad[3],
                           .texcoord = texcoords[3],
                           .color    = color };
    v[4] = (bxr_vertex_t){ .position = quad[0],
                           .texcoord = texcoords[0],
                           .color    = color };
    v[5] = (bxr_vertex_t){ .position = quad[2],
                           .texcoord = texcoords[2],
                           .color    = color };
    v += 6;
  }

  // Queue draw
  bxr_pipeline_t pipeline = _bxr_painter_lookup_pipeline(
      BXR_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);

  _bxr_painter_queue_draw(
      pipeline, region, vertex_index, total_vertices, BXR_PRIMITIVE_TRIANGLES);
}

void
bxr_painter_draw_rect_textured(int channel, bxr_textured_rect_t rect)
{
  bxr_painter_draw_rects_textured(channel, &rect, 1);
}

void
bxr_painter_draw_rects_textured(int channel,
                                const bxr_textured_rect_t *rects,
                                Uint32 count)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (count == 0) {
    return;
  }

  // Setup vertices
  Uint32 total_vertices  = count * 6; // 2 triangles per rect, 3 vertices each
  Uint32 vertex_index    = _painter.current_vertex;
  bxr_vertex_t *vertices = _bxr_painter_next_vertices(total_vertices);
  if (!vertices) {
    return;
  }

  // Get image info
  bxr_image_t image = _painter.state.texture.images[channel];

  int width  = bxr_image_get_width(image);
  int height = bxr_image_get_height(image);

  // Check image dimension with unlikely

  float iw = 1.0f / (float)width;
  float ih = 1.0f / (float)height;

  // Compute vertices
  _bxr_painter_mat2x3_t mvp = _painter.state.mvp;
  bxr_color_t color         = _painter.state.color;
  bxr_region_t region       = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

  for (Uint32 i = 0; i < count; ++i) {
    bxr_vec2_t quad[4] = {
      { rects[i].dst.x, rects[i].dst.y + rects[i].dst.h }, // bottom left
      { rects[i].dst.x + rects[i].dst.w,
        rects[i].dst.y + rects[i].dst.h },                 // bottom right
      { rects[i].dst.x + rects[i].dst.w, rects[i].dst.y }, // top right
      { rects[i].dst.x, rects[i].dst.y },                  // top left
    };

    _bxr_painter_transform(&mvp, quad, quad, 4);

    for (Uint32 j = 0; j < 4; ++j) {
      region.x0 = SDL_min(region.x0, quad[j].x);
      region.y0 = SDL_min(region.y0, quad[j].y);
      region.x1 = SDL_max(region.x1, quad[j].x);
      region.y1 = SDL_max(region.y1, quad[j].y);
    }

    float tl = rects[i].src.x * iw;
    float tt = rects[i].src.y * ih;
    float tr = (rects[i].src.x + rects[i].src.w) * iw;
    float tb = (rects[i].src.y + rects[i].src.h) * ih;

    bxr_vec2_t vtexquad[4] = {
      { tl, tb }, // bottom-left
      { tr, tb }, // bottom-right
      { tr, tt }, // top-right
      { tl, tt }  // top-left
    };

    bxr_vertex_t *v = &vertices[i * 6];
    v[0]            = (bxr_vertex_t){ .position = quad[0],
                                      .texcoord = vtexquad[0],
                                      .color    = color };
    v[1]            = (bxr_vertex_t){ .position = quad[1],
                                      .texcoord = vtexquad[1],
                                      .color    = color };
    v[2]            = (bxr_vertex_t){ .position = quad[2],
                                      .texcoord = vtexquad[2],
                                      .color    = color };
    v[3]            = (bxr_vertex_t){ .position = quad[3],
                                      .texcoord = vtexquad[3],
                                      .color    = color };
    v[4]            = (bxr_vertex_t){ .position = quad[0],
                                      .texcoord = vtexquad[0],
                                      .color    = color };
    v[5]            = (bxr_vertex_t){ .position = quad[2],
                                      .texcoord = vtexquad[2],
                                      .color    = color };
  }

  // Queue draw
  bxr_pipeline_t pipeline = _bxr_painter_lookup_pipeline(
      BXR_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);

  _bxr_painter_queue_draw(
      pipeline, region, vertex_index, total_vertices, BXR_PRIMITIVE_TRIANGLES);
}
