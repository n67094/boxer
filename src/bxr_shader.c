#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include <physfs.h>

#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_io.h"
#include "bxr_mem.h"
#include "bxr_shader.h"

typedef struct _bxr_shader_bytecode_s
{
  Uint8 *data;
  size_t size;
  SDL_GPUShaderFormat format;
} _bxr_shader_bytecode_t;

static Uint32 _initialized = 0;
static bxr_context_t *_context;

void
bxr_shader_setup(bxr_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = BXR_INIT_COOKIE;
  _context     = context;
}

void
bxr_shader_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _initialized = 0;
  _context     = NULL;
}

// Return value must be freed as well as the data inside it.
static _bxr_shader_bytecode_t *
_be_shader_load_bytecode(SDL_GPUDevice *device,
                         const char *name,
                         size_t *bytecode_size)
{
  SDL_assert(device);
  SDL_assert(name);

  _bxr_shader_bytecode_t *shader_bytecode = NULL;
  BXR_NEW(shader_bytecode);

  // Determine the shader stage/type based on the file extension
  SDL_GPUShaderStage stage;

  if (SDL_strstr(name, ".vert") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_VERTEX;
  } else if (SDL_strstr(name, ".frag") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  } else {
    bxr_set_error(BXR_ERROR_SHADER_NAME);
    return NULL;
  }

  SDL_GPUShaderFormat supported_formats = SDL_GetGPUShaderFormats(device);

  const char *file_ext = NULL;

  if (supported_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
    shader_bytecode->format = SDL_GPU_SHADERFORMAT_SPIRV;
    file_ext                = ".spv";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_MSL) {
    shader_bytecode->format = SDL_GPU_SHADERFORMAT_MSL;
    file_ext                = ".msl";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_DXIL) {
    shader_bytecode->format = SDL_GPU_SHADERFORMAT_DXIL;
    file_ext                = ".dxil";
  } else if (supported_formats & SDL_GPU_SHADERFORMAT_DXBC) {
    shader_bytecode->format = SDL_GPU_SHADERFORMAT_DXBC;
    file_ext                = ".dxbc";
  } else {
    bxr_set_error(BXR_ERROR_SHADER_FORMAT);
    return NULL;
  }

  char shader_path[BXR_PATH_MAX];
  SDL_snprintf(shader_path, BXR_PATH_MAX, "%s%s", name, file_ext);

  shader_bytecode->data = bxr_io_read(shader_path, &shader_bytecode->size);

  if (shader_bytecode->data == NULL) {
    BXR_FREE(shader_bytecode);
    bxr_set_error(BXR_ERROR_SHADER_LOAD_BYTECODE);
    return NULL;
  }

  return shader_bytecode;
}

bxr_shader_t
bxr_shader_make(bxr_shader_desc_t *desc)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _bxr_shader_bytecode_t *vert_bytecode
      = _be_shader_load_bytecode(_context->gpu_device, desc->vert_name, NULL);

  _bxr_shader_bytecode_t *frag_bytecode
      = _be_shader_load_bytecode(_context->gpu_device, desc->frag_name, NULL);

  SDL_GPShaderDesc shader_desc = {
    // Vertex shader description
    .vert_code_size            = vert_bytecode->size,
    .vert_code                 = vert_bytecode->data,
    .vert_entrypoint           = desc->vert_entrypoint,
    .vert_format               = vert_bytecode->format,
    .vert_num_samplers         = desc->vert_num_samplers,
    .vert_num_storage_textures = desc->vert_num_storage_textures,
    .vert_num_storage_buffers  = desc->vert_num_storage_buffers,
    .vert_num_uniform_buffers  = desc->vert_num_uniform_buffers,

    // Fragment shader description
    .frag_code_size            = frag_bytecode->size,
    .frag_code                 = frag_bytecode->data,
    .frag_entrypoint           = desc->frag_entrypoint,
    .frag_format               = frag_bytecode->format,
    .frag_num_samplers         = desc->frag_num_samplers,
    .frag_num_storage_textures = desc->frag_num_storage_textures,
    .frag_num_storage_buffers  = desc->frag_num_storage_buffers,
    .frag_num_uniform_buffers  = desc->frag_num_uniform_buffers,
  };

  bxr_shader_t shader = (bxr_shader_t)SDL_GPCreateShader(&shader_desc);

  BXR_FREE(vert_bytecode->data);
  BXR_FREE(vert_bytecode);

  BXR_FREE(frag_bytecode->data);
  BXR_FREE(frag_bytecode);

  return shader;
}

void
bxr_shader_destroy(bxr_shader_t shader)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_GPDestroyShader((SDL_GPShader)shader);
}
