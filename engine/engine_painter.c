#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "engine_color.h"
#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_image.h"
#include "engine_math.h"
#include "engine_painter.h"
#include "engine_pipeline.h"
#include "engine_pool.h"
#include "engine_shader.h"

typedef enum
{
  ENGINE_COMMAND_NONE = 0,
  ENGINE_COMMAND_DRAW,
  ENGINE_COMMAND_VIEWPORT,
  ENGINE_COMMAND_SCISSOR
} _engine_draw_command_type_e;

typedef struct _engine_mat2x3_s
{
  float m00, m01, m02;
  float m10, m11, m12;
} _engine_mat2x3_t;

#define engine_mat2x3_identity()                                               \
  ((const _engine_mat2x3_t){ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f })

#define engine_mat2x3_make(m00, m01, m02, m10, m11, m12)                       \
  ((const _engine_mat2x3_t){ m00, m01, m02, m10, m11, m12 })

// FIXME clean the code prefix with painter

typedef struct _engine_painter_draw_args_s
{
  engine_pipeline_t pipeline;
  engine_image_t image;
  // TODO: add region for optimization
  Uint32 uniform_index;
  Uint32 vertex_index;
  Uint32 vertices_count;
} _engine_painter_draw_args_t;

typedef union _engine_painter_command_args_s
{
  _engine_painter_draw_args_t draw;
  engine_rect_t viewport;
  engine_rect_t scissor;
} _engine_painter_command_args_t;

typedef struct _engine_painter_command_s
{
  _engine_draw_command_type_e cmd;
  _engine_painter_command_args_t args;
} _engine_painter_command_t;

typedef enum
{
  ENGINE_PAINTER_UNIFORM_SLOT_VS = 0,
  ENGINE_PAINTER_UNIFORM_SLOT_FS = 1
} _engine_painter_uniform_slot_e;

typedef union _engine_painter_uniform_data_u
{
  float floats[ENGINE_PAINTER_CONTENT_SLOTS_MAX];
  uint8_t bytes[ENGINE_PAINTER_CONTENT_SLOTS_MAX * sizeof(float)];
} _engine_painter_uniform_data_t;

typedef struct _engine_painter_uniform_s
{
  Uint16 vs_size;
  Uint16 fs_size;
  _engine_painter_uniform_data_t data;
} _engine_painter_uniform_t;

typedef struct _engine_painter_state_s
{
  _engine_mat2x3_t projection;
  _engine_mat2x3_t transform;
  _engine_mat2x3_t mvp;

  _engine_painter_uniform_t uniform;

  engine_pipeline_t pipeline;

  engine_blendmode_e blend_mode;
  engine_rect_t viewport;
  engine_rect_t scissor;
  engine_color_t color;
  engine_image_t image;
  SDL_GPUSampler *sampler;
  float thickness;

  Uint32 base_uniform;
  Uint32 base_vertex;
  Uint32 base_command;
} _engine_painter_state_t;

typedef struct _engine_painter_s
{
  SDL_GPUTransferBuffer *vertex_transfer_buffer;
  SDL_GPUBuffer *vertex_data_buffer;

  engine_shader_t shader;

  engine_pipeline_t pipelines[ENGINE_PRIMITIVE_SIZE * ENGINE_BLENDMODE_SIZE];

  SDL_GPUSampler *samplers[ENGINE_SAMPLE_SIZE];

  engine_image_t white_image;

  // States management
  Uint32 current_state;
  _engine_painter_state_t
      states[ENGINE_PAINTER_MAX]; // save states for push/pop
  _engine_painter_state_t state;  // current state

  // Transforms management
  Uint32 current_transform;
  _engine_mat2x3_t
      transforms[ENGINE_PAINTER_TRANSFORMS_MAX]; // save transforms for push/pop

  // Vertecies management
  Uint32 current_vertex;
  engine_vertex_t
      vertices[ENGINE_PAINTER_VERTICES_MAX]; // save vertecies for draw calls

  // Uniforms management
  Uint32 current_uniform;
  _engine_painter_uniform_t
      uniforms[ENGINE_PAINTER_COMMANDS_MAX]; // save uniforms for draw calls

  // Commands management
  Uint32 current_command;
  _engine_painter_command_t
      commands[ENGINE_PAINTER_COMMANDS_MAX]; // save draw commands for flush
} _engine_painter_t;

static _engine_painter_t _painter = { 0 };
static Uint32 _initialized        = 0;
static engine_context_t *_context = NULL;

static engine_pipeline_t
_engine_painter_lookup_pipeline(engine_primitive_e primitive_type,
                                engine_blendmode_e blend_mode)
{
  engine_pipeline_t pipeline
      = _painter.pipelines[primitive_type * ENGINE_BLENDMODE_SIZE + blend_mode];

  if (pipeline.id == ENGINE_INVALID_ID) {
    pipeline
        = engine_pipeline_make(_painter.shader, primitive_type, blend_mode);
    _painter.pipelines[primitive_type * ENGINE_BLENDMODE_SIZE + blend_mode]
        = pipeline;
  }

  return pipeline;
}

static SDL_GPUSamplerCreateInfo
_engine_painter_sampler(engine_sampler_e sampler_type)
{
  SDL_GPUSamplerCreateInfo sampler_info;

  SDL_memset(&sampler_info, 0, sizeof(SDL_GPUSamplerCreateInfo));

  switch (sampler_type) {
  case ENGINE_SAMPLER_POINT_WRAP:
    sampler_info.min_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mag_filter     = SDL_GPU_FILTER_NEAREST;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    break;
  case ENGINE_SAMPLER_LINEAR_CLAMP:
    sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    break;
  case ENGINE_SAMPLER_LINEAR_WRAP:
    sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    break;
  default: // ENGINE_SAMPLER_POINT_CLAMP
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

ENGINE_INLINE _engine_painter_uniform_t *
_engine_painter_next_uniform(void)
{
  if (_painter.current_uniform < ENGINE_PAINTER_COMMANDS_MAX) {
    return &_painter.uniforms[_painter.current_uniform++];
  } else {
    engine_set_error(ENGINE_ERROR_PAINTER_UNIFORMS_FULL);
    return NULL;
  }
}

ENGINE_INLINE _engine_painter_uniform_t *
_engine_painter_prev_uniform(void)
{
  if (_painter.current_uniform > 0) {
    return &_painter.uniforms[_painter.current_uniform - 1];
  } else {
    return NULL;
  }
}

ENGINE_INLINE engine_vertex_t *
_engine_painter_next_vertices(Uint32 count)
{
  if (_painter.current_vertex + count <= ENGINE_PAINTER_VERTICES_MAX) {
    engine_vertex_t *vertices = &_painter.vertices[_painter.current_vertex];
    _painter.current_vertex += count;

    return vertices;
  } else {
    engine_set_error(ENGINE_ERROR_PAINTER_VERTICES_FULL);
    return NULL;
  }
}

ENGINE_INLINE _engine_painter_command_t *
_engine_painter_next_command(void)
{
  if ((_painter.current_command < ENGINE_PAINTER_COMMANDS_MAX)) {
    return &_painter.commands[_painter.current_command++];
  } else {
    engine_set_error(ENGINE_ERROR_PAINTER_COMMANDS_FULL);
    return NULL;
  }
}

ENGINE_INLINE _engine_painter_command_t *
_engine_painter_prev_command(Uint32 count)
{
  if (_painter.current_command - _painter.state.base_command >= count) {
    return &_painter.commands[_painter.current_command - count];
  } else {
    return NULL;
  }
}

ENGINE_INLINE _engine_mat2x3_t
_engine_painter_default_projection(int width, int height)
{
  float w = (float)width;
  float h = (float)height;

  return engine_mat2x3_make(2.0f / w, 0.0f, -1.0f, 0.0f, -2.0f / h, 1.0f);
}

ENGINE_INLINE _engine_mat2x3_t
_engine_painter_mul_projection_transform(_engine_mat2x3_t *projection,
                                         _engine_mat2x3_t *transform)
{
  float x = projection->m00;
  float y = projection->m11;

  _engine_mat2x3_t out = { 0 };

  out.m00 = x * transform->m00;
  out.m01 = x * transform->m01;
  out.m02 = x * transform->m02 + projection->m02;

  out.m10 = y * transform->m10;
  out.m11 = y * transform->m11;
  out.m12 = y * transform->m12 + projection->m12;

  return out;
}

ENGINE_INLINE engine_vec2_t
_engine_painter_mat3_mul_vec2(_engine_mat2x3_t *m, const engine_vec2_t *v)
{
  return engine_vec2_make(m->m00 * v->x + m->m01 * v->y + m->m02,
                          m->m10 * v->x + m->m11 * v->y + m->m12);
}

ENGINE_INLINE void
_engine_painter_transform(_engine_mat2x3_t *matrix,
                          engine_vec2_t *dst,
                          const engine_vec2_t *src,
                          Uint32 count)
{
  for (Uint32 i = 0; i < count; ++i) {
    dst[i] = _engine_painter_mat3_mul_vec2(matrix, &src[i]);
  }
}

void
engine_painter_setup(engine_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = ENGINE_INIT_COOKIE;
  _context     = context;

  _painter.white_image = engine_image_load_mem(
      2, 2, (Uint32[]){ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF });

  if (_painter.white_image.id == ENGINE_INVALID_ID) {
    // Error is handled in engine_image_mem
    return;
  }

  // Create buffers for vertex data
  _painter.vertex_transfer_buffer = SDL_CreateGPUTransferBuffer(
      _context->gpu_device,
      &(SDL_GPUTransferBufferCreateInfo){
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size  = ENGINE_PAINTER_VERTICES_MAX * sizeof(engine_vertex_t),

      });

  if (_painter.vertex_transfer_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create vertex transfer buffer (error: %s)",
                 SDL_GetError());
    engine_set_error(ENGINE_ERROR_PAINTER_SETUP);
    return;
  }

  _painter.vertex_data_buffer = SDL_CreateGPUBuffer(
      _context->gpu_device,
      &(SDL_GPUBufferCreateInfo){
          .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
          .size  = ENGINE_PAINTER_VERTICES_MAX * sizeof(engine_vertex_t),
      });
  if (_painter.vertex_data_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create vertex data buffer (error: %s)",
                 SDL_GetError());
    engine_set_error(ENGINE_ERROR_PAINTER_SETUP);
    return;
  }

  _painter.shader = engine_shader_make(
      &(engine_shader_desc_t){ .name         = "data/shaders/painter.vert",
                               .num_samplers = 0,
                               .num_uniform_buffers  = 0,
                               .num_storage_buffers  = 0,
                               .num_storage_textures = 0 },
      &(engine_shader_desc_t){ .name         = "data/shaders/painter.frag",
                               .num_samplers = 1,
                               .num_uniform_buffers  = 0,
                               .num_storage_buffers  = 0,
                               .num_storage_textures = 0 });

  bool pips_ok = true;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_POINTS,
                                             ENGINE_BLENDMODE_NONE)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_POINTS,
                                             ENGINE_BLENDMODE_BLEND)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_LINES,
                                             ENGINE_BLENDMODE_NONE)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_LINES,
                                             ENGINE_BLENDMODE_BLEND)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_LINE_STRIP,
                                             ENGINE_BLENDMODE_NONE)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_LINE_STRIP,
                                             ENGINE_BLENDMODE_BLEND)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_TRIANGLES,
                                             ENGINE_BLENDMODE_NONE)
                 .id
             != ENGINE_INVALID_ID;
  pips_ok &= _engine_painter_lookup_pipeline(ENGINE_PRIMITIVE_TRIANGLES,
                                             ENGINE_BLENDMODE_BLEND)
                 .id
             != ENGINE_INVALID_ID;

  if (!pips_ok) {
    engine_painter_shutdown();
    engine_set_error(ENGINE_ERROR_PAINTER_SETUP);
    return;
  }

  for (int i = 0; i < ENGINE_SAMPLE_SIZE; ++i) {
    SDL_GPUSamplerCreateInfo sampler_info
        = _engine_painter_sampler((engine_sampler_e)i);
    SDL_GPUSampler *sampler
        = SDL_CreateGPUSampler(_context->gpu_device, &sampler_info);

    if (sampler == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Failed to create sampler (error: %s)",
                   SDL_GetError());
      engine_set_error(ENGINE_ERROR_PAINTER_SETUP);
      return;
    }

    _painter.samplers[i] = sampler;
  }
}

void
engine_painter_shutdown()
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  SDL_ReleaseGPUTransferBuffer(_context->gpu_device,
                               _painter.vertex_transfer_buffer);
  SDL_ReleaseGPUBuffer(_context->gpu_device, _painter.vertex_data_buffer);

  for (int i = 0; i < ENGINE_PRIMITIVE_SIZE * ENGINE_BLENDMODE_SIZE; ++i) {
    if (_painter.pipelines[i].id != ENGINE_INVALID_ID) {
      engine_pipeline_destroy(_painter.pipelines[i]);
      _painter.pipelines[i] = (engine_pipeline_t){ .id = ENGINE_INVALID_ID };
    }
  }

  engine_shader_destroy(_painter.shader);

  for (int i = 0; i < ENGINE_SAMPLE_SIZE; ++i) {
    SDL_ReleaseGPUSampler(_context->gpu_device, _painter.samplers[i]);
  }

  engine_image_destroy(_painter.white_image);

  _initialized = 0;
  _context     = NULL;
}

void
engine_painter_begin(int width, int height)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  // Save current state
  _painter.states[_painter.current_state++] = _painter.state;

  _painter.state.viewport   = (engine_rect_t){ 0, 0, width, height };
  _painter.state.scissor    = (engine_rect_t){ 0, 0, -1, -1 };
  _painter.state.color      = ENGINE_COLOR_WHITE;
  _painter.state.projection = _engine_painter_default_projection(width, height);
  _painter.state.transform  = engine_mat2x3_identity();
  _painter.state.mvp        = _painter.state.projection;
  _painter.state.image      = _painter.white_image;
  _painter.state.sampler    = _painter.samplers[ENGINE_SAMPLER_POINT_CLAMP];
  _painter.state.blend_mode = ENGINE_BLENDMODE_NONE;
  _painter.state.thickness  = SDL_max(1.0f / width, 1.0f / height);
  _painter.state.base_vertex  = _painter.current_vertex;
  _painter.state.base_command = _painter.current_command;
}

void
engine_painter_flush()
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
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
  engine_vertex_t *vertex_data = (engine_vertex_t *)SDL_MapGPUTransferBuffer(
      _context->gpu_device, _painter.vertex_transfer_buffer, true);
  if (vertex_data == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to map vertex transfer buffer (error: %s)",
                 SDL_GetError());

    engine_set_error(ENGINE_ERROR_PAINTER_FLUSH);
    return;
  }

  SDL_memcpy(vertex_data,
             &_painter.vertices[_painter.state.base_vertex],
             vertex_count * sizeof(engine_vertex_t));

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
          .offset = _painter.state.base_vertex * sizeof(engine_vertex_t),
          .size   = (Uint32)((vertex_count) * sizeof(engine_vertex_t)) },
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

  Uint32 cur_pipeline_id   = ENGINE_IMPOSSIBLE_ID;
  Uint32 cur_uniform_index = ENGINE_IMPOSSIBLE_ID;
  Uint32 cur_image_id      = ENGINE_IMPOSSIBLE_ID;

  // Flush commands
  for (Uint32 i = _painter.state.base_command; i < end_command; ++i) {
    _engine_painter_command_t *cmd = &_painter.commands[i];

    // Forward declartion
    SDL_Rect scissor         = { 0 };
    SDL_GPUViewport viewport = { 0 };

    switch (cmd->cmd) {
    case ENGINE_COMMAND_DRAW: {
      bool rebind_uniforms = false;
      bool rebind_texture  = false;

      // Check if pipeline needs to be changed and bind it
      if (cmd->args.draw.pipeline.id != cur_pipeline_id) {
        cur_pipeline_id = cmd->args.draw.pipeline.id;

        // Bind pipeline
        SDL_BindGPUGraphicsPipeline(
            render_pass, engine_pipeline_get(cmd->args.draw.pipeline));

        // When pipeline changes we need to rebind uniforms and textures
        rebind_uniforms = true;
        rebind_texture  = true;
      }

      if (cmd->args.draw.image.id != cur_image_id) {
        cur_image_id = cmd->args.draw.image.id;

        // When image changes we need to rebind textures
        rebind_texture = true;
      }

      if (cmd->args.draw.uniform_index != cur_uniform_index) {
        cur_uniform_index = cmd->args.draw.uniform_index;

        // When uniform index changes we need to rebind uniforms
        rebind_uniforms = true;
      }

      // Rebind texture if needed
      if (rebind_texture) {
        SDL_GPUTexture *texture
            = engine_image_get_texture(cmd->args.draw.image);
        // TODO check texture validity

        SDL_BindGPUFragmentSamplers(
            render_pass,
            0,
            &(SDL_GPUTextureSamplerBinding){
                .texture = texture, .sampler = _painter.state.sampler },
            1);
      }

      // Rebind uniforms if needed
      if (rebind_uniforms && cur_uniform_index != ENGINE_IMPOSSIBLE_ID) {
        _engine_painter_uniform_t *uniform
            = &_painter.uniforms[cmd->args.draw.uniform_index];

        if (uniform->vs_size > 0) {
          SDL_PushGPUVertexUniformData(_context->cmd_buffer,
                                       ENGINE_PAINTER_UNIFORM_SLOT_VS,
                                       &uniform->data.bytes[0],
                                       uniform->vs_size);
        }
        if (uniform->fs_size > 0) {
          SDL_PushGPUFragmentUniformData(_context->cmd_buffer,
                                         ENGINE_PAINTER_UNIFORM_SLOT_FS,
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
                                             * sizeof(engine_vertex_t) },
          1);

      SDL_DrawGPUPrimitives(
          render_pass, cmd->args.draw.vertices_count, 1, 0, 0);
      break;
    }
    case ENGINE_COMMAND_VIEWPORT:
      viewport = (SDL_GPUViewport){
        .x = (float)cmd->args.viewport.x,
        .y = (float)cmd->args.viewport.y,
        .w = (float)cmd->args.viewport.w,
        .h = (float)cmd->args.viewport.h,
      };
      SDL_SetGPUViewport(render_pass, &viewport);
      break;
    case ENGINE_COMMAND_SCISSOR:
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
engine_painter_end(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _painter.state = _painter.states[--_painter.current_state];
}

engine_vec2_t
engine_painter_dimensions(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  return engine_vec2_make(_painter.state.viewport.w, _painter.state.viewport.h);
}

void
engine_painter_set_projection(float left, float right, float bottom, float top)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  float width  = right - left;
  float height = top - bottom;

  _painter.state.projection = engine_mat2x3_make(2.0f / width,
                                                 0.0f,
                                                 -(right + left) / width,
                                                 0.0f,
                                                 2.0f / height,
                                                 -(top + bottom) / height);

  _painter.state.mvp = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_reset_projection(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.projection = _engine_painter_default_projection(
      (int)_painter.state.viewport.w, (int)_painter.state.viewport.h);

  _painter.state.mvp = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_push_transform(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);
  SDL_assert(_painter.current_transform < ENGINE_PAINTER_TRANSFORMS_MAX);

  _painter.transforms[_painter.current_transform++] = _painter.state.transform;
}

void
engine_painter_pop_transform(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);
  SDL_assert(_painter.current_transform > 0);

  _painter.state.transform = _painter.transforms[--_painter.current_transform];
  _painter.state.mvp       = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_reset_transform(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.transform = engine_mat2x3_identity();
  _painter.state.mvp       = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_translate(float x, float y)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // multiply by translate matrix:
  // 1.0f, 0.0f, tx,
  // 0.0f, 1.0f, ty,

  _painter.state.transform.m02
      += x * _painter.state.transform.m00 + y * _painter.state.transform.m01;
  _painter.state.transform.m12
      += x * _painter.state.transform.m10 + y * _painter.state.transform.m11;

  _painter.state.mvp = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_rotate(float angle)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  float c = engine_cos(angle);
  float s = engine_sin(angle);

  // Multiply by rotation matrix:
  //   c,   -s, 0.0f,
  //   s,    c, 0.0f,

  _engine_mat2x3_t rotation = engine_mat2x3_make(
      c * _painter.state.transform.m00 + s * _painter.state.transform.m01,
      -s * _painter.state.transform.m00 + c * _painter.state.transform.m01,
      _painter.state.transform.m02,
      c * _painter.state.transform.m10 + s * _painter.state.transform.m11,
      -s * _painter.state.transform.m10 + c * _painter.state.transform.m11,
      _painter.state.transform.m12);

  _painter.state.transform = rotation;
  _painter.state.mvp       = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_rotate_at(float angle, float ax, float ay)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  engine_painter_translate(ax, ay);
  engine_painter_rotate(angle);
  engine_painter_translate(-ax, -ay);
}

void
engine_painter_scale(float sx, float sy)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // Multiply by scale matrix:
  //   sx, 0.0f, 0.0f,
  // 0.0f,   sy, 0.0f,

  _painter.state.transform.m00 *= sx;
  _painter.state.transform.m01 *= sy;
  _painter.state.transform.m10 *= sx;
  _painter.state.transform.m11 *= sy;

  _painter.state.mvp = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_scale_at(float sx, float sy, float ax, float ay)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  engine_painter_translate(ax, ay);
  engine_painter_scale(sx, sy);
  engine_painter_translate(-ax, -ay);
}

void
engine_painter_set_pipeline(engine_pipeline_t pipeline)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  _painter.state.pipeline = pipeline;

  // Reset uniforms when pipeline changes
  SDL_memset(&_painter.state.uniform, 0, sizeof(_engine_painter_uniform_t));
}

void
engine_painter_reset_pipeline(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  engine_pipeline_t pipeline = { .id = ENGINE_INVALID_ID };

  engine_painter_set_pipeline(pipeline);
}

void
engine_painter_set_uniforms(const void *vs_data,
                            size_t vs_size,
                            const void *fs_data,
                            size_t fs_size)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.state.pipeline.id != ENGINE_INVALID_ID);

  size_t size = vs_size + fs_size;

  SDL_assert(size <= ENGINE_PAINTER_CONTENT_SLOTS_MAX * sizeof(float));

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
engine_painter_reset_uniforms(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.state.pipeline.id != ENGINE_INVALID_ID);
  engine_painter_set_uniforms(NULL, 0, NULL, 0);
}

void
engine_painter_set_blend_mode(engine_blendmode_e blend_mode)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.blend_mode = blend_mode;
}

void
engine_painter_reset_blend_mode(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.blend_mode = ENGINE_BLENDMODE_BLEND;
}

void
engine_painter_set_color(engine_color_t color)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.color = color;
}

engine_color_t
engine_painter_get_color(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  return _painter.state.color;
}

void
engine_painter_reset_color(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.color = ENGINE_COLOR_WHITE;
}

void
engine_painter_set_thickness(float thickness)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.thickness = thickness;
}

void
engine_painter_reset_thickness(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.thickness = SDL_max(1.0f / _painter.state.viewport.w,
                                     1.0f / _painter.state.viewport.h);
}

void
engine_painter_set_image(engine_image_t image)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.image = image;
}

engine_image_t
engine_painter_get_image(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  return _painter.state.image;
}

void
engine_painter_reset_image(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.image = _painter.white_image;
}

void
engine_painter_set_sampler(engine_sampler_e sampler)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.sampler = _painter.samplers[sampler];
}

void
engine_painter_reset_sampler(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.sampler = _painter.samplers[ENGINE_SAMPLER_POINT_CLAMP];
}

void
engine_painter_viewport(int x, int y, int w, int h)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // No change
  if (_painter.state.viewport.x == x && _painter.state.viewport.y == y
      && _painter.state.viewport.w == w && _painter.state.viewport.h == h) {
    return;
  }

  // Try to reuse previous command
  _engine_painter_command_t *cmd = _engine_painter_prev_command(1);
  if (cmd && cmd->cmd != ENGINE_COMMAND_VIEWPORT) {
    cmd = _engine_painter_next_command();
  }

  if (!cmd) {
    return;
  }

  engine_rect_t viewport
      = engine_rect_make((float)x, (float)y, (float)w, (float)h);

  SDL_memset(&cmd->args.viewport, 0, sizeof(engine_rect_t));

  cmd->cmd           = ENGINE_COMMAND_VIEWPORT;
  cmd->args.viewport = viewport;

  // TODO scissor

  _painter.state.viewport   = viewport;
  _painter.state.thickness  = ENGINE_MAX(1.0f / w, 1.0f / h);
  _painter.state.projection = _engine_painter_default_projection(
      (int)_painter.state.viewport.w, (int)_painter.state.viewport.h);
  _painter.state.mvp = _engine_painter_mul_projection_transform(
      &_painter.state.projection, &_painter.state.transform);
}

void
engine_painter_reset_viewport(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  engine_painter_viewport(
      0, 0, (float)_context->config.width, (float)_context->config.height);
}

void
engine_painter_scissor(int x, int y, int w, int h)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.scissor
      = engine_rect_make((float)x, (float)y, (float)w, (float)h);
}

void
engine_painter_reset_scissor(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  _painter.state.scissor = (engine_rect_t){ 0, 0, -1, -1 };
}

void
engine_painter_reset_state(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  engine_painter_reset_projection();
  engine_painter_reset_transform();
  engine_painter_reset_blend_mode();
  engine_painter_reset_color();
  engine_painter_reset_image();
  engine_painter_reset_sampler();
  engine_painter_reset_viewport();
  engine_painter_reset_scissor();
}

static bool
_engine_painter_merge_draw_commands(SDL_GPUGraphicsPipeline *pipeline,
                                    engine_image_t image,
                                    Uint32 vertex_index,
                                    Uint32 vertices_count,
                                    SDL_GPUPrimitiveType primitive_type)
{
#if ENGINE_PAINTER_OPTIMIZE_DEPTH > 0
  _engine_command_t *prev_cmd = NULL;
  _engine_command_t *inter_cmds[ENGINE_PAINTER_OPTIMIZE_DEPTH];
  Uint32 inter_count = 0;

  // Find a command that is mergeable
  Uint32 lookup_depht = ENGINE_PAINTER_OPTIMIZE_DEPTH;
  for (Uint32 depth = 1; depth <= lookup_depht; ++depth) {
    _engine_command_t *cmd = _engine_prev_command(depth + 1);

    // Stop on nonexistent command
    if (!cmd) {
      break;
    }

    // command was optimized awaw, continue looking
    if (cmd->cmd == ENGINE_COMMAND_NONE) {
      lookup_depht++;
      continue;
    }

    // Stop on scissor or viewport
    if (cmd->cmd != ENGINE_COMMAND_DRAW) {
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
_engine_painter_queue_draw(engine_pipeline_t pipeline,
                           engine_region_t region,
                           Uint32 vertex_index,
                           Uint32 vertices_count,
                           engine_primitive_e primitive_type)
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
      && _engine_painter_merge_draw_commands(pipeline,
                                         _state.image,
                                         vertex_index,
                                         vertices_count,
                                         primitive_type)) {
    return;
  }
  */

  _engine_painter_uniform_t *uniform = NULL;
  if (_painter.state.pipeline.id != ENGINE_INVALID_ID) {
    pipeline = _painter.state.pipeline;
    uniform  = &_painter.state.uniform;
  }

  Uint32 uniform_index = ENGINE_IMPOSSIBLE_ID;

  if (uniform) {
    _engine_painter_uniform_t *prev_uniform = _engine_painter_prev_uniform();

    bool reuse_uniform
        = prev_uniform
          && (SDL_memcmp(
                  prev_uniform, uniform, sizeof(_engine_painter_uniform_t))
              == 0);

    if (!reuse_uniform) {
      _engine_painter_uniform_t *next_uniform = _engine_painter_next_uniform();
      if (!next_uniform) {
        _painter.current_vertex
            -= vertices_count; // rollback allocated vertices
        return;
      }
      *next_uniform = _painter.state.uniform;
    }

    uniform_index = _painter.current_uniform
                    - 1; // - 1 since _engine_painter_next_uniform already
                         // incremented the index
  }

  // New draw command
  _engine_painter_command_t *cmd = _engine_painter_next_command();

  if (!cmd) {
    _painter.current_vertex -= vertices_count; // rollback allocated vertices
    return;
  }

  cmd->cmd                = ENGINE_COMMAND_DRAW;
  cmd->args.draw.pipeline = pipeline;
  cmd->args.draw.image    = _painter.state.image;
  // TODO region for optimization
  cmd->args.draw.uniform_index  = uniform_index;
  cmd->args.draw.vertex_index   = vertex_index;
  cmd->args.draw.vertices_count = vertices_count;
}

static void
_engine_painter_draw_solid(engine_primitive_e primitive_type,
                           const engine_vec2_t *vertices,
                           Uint32 vertices_count)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (vertices_count == 0) {
    return;
  }

  // Setup vertices
  Uint32 vertex_index = _painter.current_vertex;
  engine_vertex_t *v  = _engine_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  float thickness        = (primitive_type == ENGINE_PRIMITIVE_POINTS
                     || primitive_type == ENGINE_PRIMITIVE_LINES
                     || primitive_type == ENGINE_PRIMITIVE_LINE_STRIP)
                               ? _painter.state.thickness
                               : 1.0f;
  engine_color_t color   = _painter.state.color;
  _engine_mat2x3_t mvp   = _painter.state.mvp;
  engine_region_t region = { FLOAT_MAX, FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

  for (Uint32 i = 0; i < vertices_count; ++i) {
    engine_vec2_t p = _engine_painter_mat3_mul_vec2(&mvp, &vertices[i]);

    region.x0 = ENGINE_MIN(region.x0, p.x - thickness);
    region.y0 = ENGINE_MIN(region.y0, p.y - thickness);
    region.x1 = ENGINE_MAX(region.x1, p.x + thickness);
    region.y1 = ENGINE_MAX(region.y1, p.y + thickness);

    v[i].position = p;
    v[i].texcoord = engine_vec2_make(0.0f, 0.0f);
    v[i].color    = color;
  }

  engine_pipeline_t pipeline = _engine_painter_lookup_pipeline(
      primitive_type, _painter.state.blend_mode);

  // Queue draw
  _engine_painter_queue_draw(
      pipeline, region, vertex_index, vertices_count, primitive_type);
}

void
engine_painter_clear(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  // Setup vertices
  Uint32 vertices_count = 6;
  Uint32 vertex_index   = _painter.current_vertex;

  engine_vertex_t *v = _engine_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  // Compute vertices
  const engine_vec2_t quad[4] = {
    { -1.0f, -1.0f }, // bottom-left
    { 1.0f, -1.0f },  // bottom-right
    { 1.0f, 1.0f },   // top-right
    { -1.0f, 1.0f }   // top-left
  };

  const engine_vec2_t texcoord = { 0.0f, 0.0f };
  engine_color_t color         = _painter.state.color;

  v[0] = (engine_vertex_t){ .position = quad[0],
                            .texcoord = texcoord,
                            .color    = color };
  v[1] = (engine_vertex_t){ .position = quad[1],
                            .texcoord = texcoord,
                            .color    = color };
  v[2] = (engine_vertex_t){ .position = quad[2],
                            .texcoord = texcoord,
                            .color    = color };
  v[3] = (engine_vertex_t){ .position = quad[2],
                            .texcoord = texcoord,
                            .color    = color };
  v[4] = (engine_vertex_t){ .position = quad[3],
                            .texcoord = texcoord,
                            .color    = color };
  v[5] = (engine_vertex_t){ .position = quad[0],
                            .texcoord = texcoord,
                            .color    = color };

  engine_region_t region = { -1.0f, -1.0f, 1.0f, 1.0f };

  engine_pipeline_t pipeline = _engine_painter_lookup_pipeline(
      ENGINE_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);

  _engine_painter_queue_draw(pipeline,
                             region,
                             vertex_index,
                             vertices_count,
                             ENGINE_PRIMITIVE_TRIANGLES);
}

void
engine_painter_draw(engine_primitive_e primitive_type,
                    const engine_vertex_t *vertices,
                    Uint32 vertices_count)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (vertices_count == 0) {
    return;
  }

  // Setup vertices
  Uint32 vertex_index = _painter.current_vertex;
  engine_vertex_t *v  = _engine_painter_next_vertices(vertices_count);
  if (!v) {
    return;
  }

  float thickness        = (primitive_type == ENGINE_PRIMITIVE_POINTS
                     || primitive_type == ENGINE_PRIMITIVE_LINES
                     || primitive_type == ENGINE_PRIMITIVE_LINE_STRIP)
                               ? _painter.state.thickness
                               : 1.0f;
  _engine_mat2x3_t mvp   = _painter.state.mvp;
  engine_region_t region = { FLOAT_MAX, FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

  for (Uint32 i = 0; i < vertices_count; ++i) {
    engine_vec2_t p
        = _engine_painter_mat3_mul_vec2(&mvp, &vertices[i].position);

    region.x0 = ENGINE_MIN(region.x0, p.x - thickness);
    region.y0 = ENGINE_MIN(region.y0, p.y - thickness);
    region.x1 = ENGINE_MAX(region.x1, p.x + thickness);
    region.y1 = ENGINE_MAX(region.y1, p.y + thickness);

    v[i].position = p;
    v[i].texcoord = vertices[i].texcoord;
    v[i].color    = vertices[i].color;
  }

  engine_pipeline_t pipeline = _engine_painter_lookup_pipeline(
      primitive_type, _painter.state.blend_mode);

  // Queue draw
  _engine_painter_queue_draw(
      pipeline, region, vertex_index, vertices_count, primitive_type);
}

void
engine_painter_draw_points(const engine_vec2_t *points, Uint32 count)
{
  _engine_painter_draw_solid(ENGINE_PRIMITIVE_POINTS, points, count);
}

void
engine_painter_draw_point(engine_vec2_t point)
{
  engine_painter_draw_points(&point, 1);
}

void
engine_painter_draw_lines(const engine_line_t *lines, Uint32 count)
{
  _engine_painter_draw_solid(
      ENGINE_PRIMITIVE_LINES, (const engine_vec2_t *)lines, count * 2);
}

void
engine_painter_draw_line(engine_line_t line)
{
  engine_painter_draw_lines(&line, 1);
}

void
engine_painter_draw_lines_strip(const engine_vec2_t *points, Uint32 count)
{
  _engine_painter_draw_solid(
      ENGINE_PRIMITIVE_LINE_STRIP, (const engine_vec2_t *)points, count);
}

void
engine_painter_draw_triangles(const engine_triangle_t *triangles, Uint32 count)
{
  _engine_painter_draw_solid(
      ENGINE_PRIMITIVE_TRIANGLES, (const engine_vec2_t *)triangles, count * 3);
}

void
engine_painter_draw_triangle_filled(engine_triangle_t triangle)
{
  engine_painter_draw_triangles(&triangle, 1);
}

void
engine_painter_draw_triangles_strip(const engine_vec2_t *points, Uint32 count)
{
  _engine_painter_draw_solid(
      ENGINE_PRIMITIVE_TRIANGLE_STRIP, (const engine_vec2_t *)points, count);
}

void
engine_painter_draw_rect_filled(engine_rect_t rect)
{
  engine_painter_draw_rects_filled(&rect, 1);
}

void
engine_painter_draw_rects_filled(const engine_rect_t *rects, Uint32 count)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (count == 0) {
    return;
  }

  // Setup vertices
  Uint32 total_vertices = count * 6; // 2 triangles per rect, 3 vertices each
  Uint32 vertex_index   = _painter.current_vertex;
  engine_vertex_t *v    = _engine_painter_next_vertices(total_vertices);
  if (!v) {
    return;
  }

  // Compute vertices
  const engine_rect_t *rect = rects;
  engine_color_t color      = _painter.state.color;
  _engine_mat2x3_t mvp      = _painter.state.mvp;
  engine_region_t region    = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

  for (Uint32 i = 0; i < count; ++i, ++rect) {
    engine_vec2_t quad[4] = {
      { rect->x, rect->y + rect->h },           // bottom-left
      { rect->x + rect->w, rect->y + rect->h }, // bottom-right
      { rect->x + rect->w, rect->y },           // top-right
      { rect->x, rect->y },                     // top-left
    };

    _engine_painter_transform(&mvp, quad, quad, 4);

    for (Uint32 j = 0; j < 4; ++j) {
      region.x0 = SDL_min(region.x0, quad[j].x);
      region.y0 = SDL_min(region.y0, quad[j].y);
      region.x1 = SDL_max(region.x1, quad[j].x);
      region.y1 = SDL_max(region.y1, quad[j].y);
    }

    const engine_vec2_t texcoords[4] = {
      { 0.0f, 1.0f }, // bottom-left
      { 1.0f, 1.0f }, // bottom-right
      { 1.0f, 0.0f }, // top-right
      { 0.0f, 0.0f }  // top-left
    };

    // Make two triangles to form the quad
    v[0] = (engine_vertex_t){ .position = quad[0],
                              .texcoord = texcoords[0],
                              .color    = color };
    v[1] = (engine_vertex_t){ .position = quad[1],
                              .texcoord = texcoords[1],
                              .color    = color };
    v[2] = (engine_vertex_t){ .position = quad[2],
                              .texcoord = texcoords[2],
                              .color    = color };
    v[3] = (engine_vertex_t){ .position = quad[3],
                              .texcoord = texcoords[3],
                              .color    = color };
    v[4] = (engine_vertex_t){ .position = quad[0],
                              .texcoord = texcoords[0],
                              .color    = color };
    v[5] = (engine_vertex_t){ .position = quad[2],
                              .texcoord = texcoords[2],
                              .color    = color };
    v += 6;
  }

  // Queue draw
  engine_pipeline_t pipeline = _engine_painter_lookup_pipeline(
      ENGINE_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);

  _engine_painter_queue_draw(pipeline,
                             region,
                             vertex_index,
                             total_vertices,
                             ENGINE_PRIMITIVE_TRIANGLES);
}

void
engine_painter_draw_rect_textured(engine_textured_rect_t rect)
{
  engine_painter_draw_rects_textured(&rect, 1);
}

void
engine_painter_draw_rects_textured(const engine_textured_rect_t *rects,
                                   Uint32 count)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_painter.current_state > 0);

  if (count == 0) {
    return;
  }

  // Setup vertices
  Uint32 total_vertices = count * 6; // 2 triangles per rect, 3 vertices each
  Uint32 vertex_index   = _painter.current_vertex;
  engine_vertex_t *vertices = _engine_painter_next_vertices(total_vertices);
  if (!vertices) {
    return;
  }

  // Get image info
  engine_image_t image = _painter.state.image;

  int width  = engine_image_get_width(image);
  int height = engine_image_get_height(image);

  // Check image dimension with unlikely

  float iw = 1.0f / (float)width;
  float ih = 1.0f / (float)height;

  // Compute vertices
  _engine_mat2x3_t mvp   = _painter.state.mvp;
  engine_color_t color   = _painter.state.color;
  engine_region_t region = { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

  for (Uint32 i = 0; i < count; ++i) {
    engine_vec2_t quad[4] = {
      { rects[i].dst.x, rects[i].dst.y + rects[i].dst.h }, // bottom left
      { rects[i].dst.x + rects[i].dst.w,
        rects[i].dst.y + rects[i].dst.h },                 // bottom right
      { rects[i].dst.x + rects[i].dst.w, rects[i].dst.y }, // top right
      { rects[i].dst.x, rects[i].dst.y },                  // top left
    };

    _engine_painter_transform(&mvp, quad, quad, 4);

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

    engine_vec2_t vtexquad[4] = {
      { tl, tb }, // bottom-left
      { tr, tb }, // bottom-right
      { tr, tt }, // top-right
      { tl, tt }  // top-left
    };

    engine_vertex_t *v = &vertices[i * 6];
    v[0]               = (engine_vertex_t){ .position = quad[0],
                                            .texcoord = vtexquad[0],
                                            .color    = color };
    v[1]               = (engine_vertex_t){ .position = quad[1],
                                            .texcoord = vtexquad[1],
                                            .color    = color };
    v[2]               = (engine_vertex_t){ .position = quad[2],
                                            .texcoord = vtexquad[2],
                                            .color    = color };
    v[3]               = (engine_vertex_t){ .position = quad[3],
                                            .texcoord = vtexquad[3],
                                            .color    = color };
    v[4]               = (engine_vertex_t){ .position = quad[0],
                                            .texcoord = vtexquad[0],
                                            .color    = color };
    v[5]               = (engine_vertex_t){ .position = quad[2],
                                            .texcoord = vtexquad[2],
                                            .color    = color };
  }

  // Queue draw
  engine_pipeline_t pipeline = _engine_painter_lookup_pipeline(
      ENGINE_PRIMITIVE_TRIANGLES, _painter.state.blend_mode);
  _engine_painter_queue_draw(pipeline,
                             region,
                             vertex_index,
                             total_vertices,
                             ENGINE_PRIMITIVE_TRIANGLES);
}
