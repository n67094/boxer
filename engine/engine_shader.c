#include <SDL3/SDL.h>

#include <physfs.h>

#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_io.h"
#include "engine_mem.h"
#include "engine_pool.h"
#include "engine_shader.h"

typedef struct _engine_shader_s
{
  SDL_GPUShader *fragment;
  SDL_GPUShader *vertex;
  // TODO add compute shader support ?
} _engine_shader_t;

static engine_pool_t *_shader_pool;
static _engine_shader_t *_shaders;

static Uint32 _initialized = 0;
static engine_context_t *_context;

static SDL_GPUShader *
_be_shader_load(SDL_GPUDevice *device,
                const char *name,
                Uint32 num_samplers,
                Uint32 num_storage_textures,
                Uint32 num_storage_buffers,
                Uint32 num_uniform_buffers)
{
  SDL_assert(device);
  SDL_assert(name);

  // Determine the shader stage/type based on the file extension
  SDL_GPUShaderStage stage;

  if (SDL_strstr(name, ".vert") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_VERTEX;
  } else if (SDL_strstr(name, ".frag") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  } else {
    engine_set_error(ENGINE_ERROR_SHADER_LOAD);
    return NULL;
  }

  SDL_GPUShaderFormat supported_formats = SDL_GetGPUShaderFormats(device);
  SDL_GPUShaderFormat format;

  const char *file_ext = NULL;

  if (supported_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
    format   = SDL_GPU_SHADERFORMAT_SPIRV;
    file_ext = ".spv";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_MSL) {
    format   = SDL_GPU_SHADERFORMAT_MSL;
    file_ext = ".msl";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_DXIL) {
    format   = SDL_GPU_SHADERFORMAT_DXIL;
    file_ext = ".dxil";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_DXBC) {
    format   = SDL_GPU_SHADERFORMAT_DXBC;
    file_ext = ".dxbc";
  } else {
    engine_set_error(ENGINE_ERROR_SHADER_LOAD);
    return NULL;
  }

  char shader_path[ENGINE_PATH_MAX];
  SDL_snprintf(shader_path, ENGINE_PATH_MAX, "%s%s", name, file_ext);

  size_t bytecode_size = 0;
  Uint8 *bytecode      = (Uint8 *)engine_io_read(shader_path, &bytecode_size);

  if (bytecode == NULL) {
    engine_set_error(ENGINE_ERROR_SHADER_LOAD);
    return NULL;
  }

  // Create the shader from the bytecode
  SDL_GPUShader *shader
      = SDL_CreateGPUShader(device,
                            &(SDL_GPUShaderCreateInfo){
                                .code                 = bytecode,
                                .code_size            = bytecode_size,
                                .entrypoint           = "main",
                                .format               = format,
                                .stage                = stage,
                                .num_samplers         = num_samplers,
                                .num_storage_textures = num_storage_textures,
                                .num_storage_buffers  = num_storage_buffers,
                                .num_uniform_buffers  = num_uniform_buffers,
                            });

  ENGINE_FREE(bytecode);

  if (shader == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create shader from file: %s (error: %s)",
                 shader_path,
                 SDL_GetError());

    engine_set_error(ENGINE_ERROR_SHADER_LOAD);
    return NULL;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "Loaded shaders: %s (stage: %d, format: %d)",
              shader_path,
              stage,
              format);

  return shader;
}

void
engine_shader_setup(engine_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = ENGINE_INIT_COOKIE;
  _context     = context;

  _shader_pool = engine_pool_make(ENGINE_RESOURCES_SHADER_MAX);
  ENGINE_CALLOC(
      _shaders, ENGINE_RESOURCES_SHADER_MAX, sizeof(_engine_shader_t));
}

void
engine_shader_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  engine_pool_destroy(_shader_pool);
  ENGINE_FREE(_shaders);

  _initialized = 0;
  _context     = NULL;
}

engine_shader_t
engine_shader_make(engine_shader_desc_t *vertex_desc,
                   engine_shader_desc_t *fragment_desc)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(vertex_desc->name);
  SDL_assert(SDL_strstr(vertex_desc->name, ".vert") != NULL);
  SDL_assert(fragment_desc->name);
  SDL_assert(SDL_strstr(fragment_desc->name, ".frag") != NULL);

  SDL_GPUShader *vertex_shader
      = _be_shader_load(_context->device,
                        vertex_desc->name,
                        vertex_desc->num_samplers,
                        vertex_desc->num_storage_textures,
                        vertex_desc->num_storage_buffers,
                        vertex_desc->num_uniform_buffers);
  if (vertex_shader == NULL) {
    return (engine_shader_t){ .id = ENGINE_INVALID_ID };
  }

  SDL_GPUShader *fragment_shader
      = _be_shader_load(_context->device,
                        fragment_desc->name,
                        fragment_desc->num_samplers,
                        fragment_desc->num_storage_textures,
                        fragment_desc->num_storage_buffers,
                        fragment_desc->num_uniform_buffers);

  if (fragment_shader == NULL) {
    SDL_ReleaseGPUShader(_context->device, vertex_shader);
    return (engine_shader_t){ .id = ENGINE_INVALID_ID };
  }

  int slot_index = engine_pool_acquire_slot(_shader_pool);
  if (slot_index == _ENGINE_INVALID_SLOT) {
    engine_set_error(ENGINE_ERROR_SHADER_MAKE);
    return (engine_shader_t){ .id = ENGINE_INVALID_ID };
  }

  _shaders[slot_index] = (_engine_shader_t){
    .fragment = fragment_shader,
    .vertex   = vertex_shader,
  };

  return (engine_shader_t){ .id
                            = engine_pool_gen_id(_shader_pool, slot_index) };
}

SDL_GPUShader *
engine_shader_get_vertex(engine_shader_t shader)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_id_to_slot(shader.id);
  return _shaders[slot_index].vertex;
}

SDL_GPUShader *
engine_shader_get_fragment(engine_shader_t shader)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_id_to_slot(shader.id);
  return _shaders[slot_index].fragment;
}

void
engine_shader_destroy(engine_shader_t shader)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  int slot_index = engine_pool_id_to_slot(shader.id);
  engine_pool_release_slot(_shader_pool, slot_index);

  _engine_shader_t inner_shader = _shaders[slot_index];

  SDL_ReleaseGPUShader(_context->device, inner_shader.vertex);
  SDL_ReleaseGPUShader(_context->device, inner_shader.fragment);

  _shaders[slot_index] = (_engine_shader_t){
    .vertex   = NULL,
    .fragment = NULL,
  };
}
