#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include <physfs.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_log.h"
#include "bxr_mem.h"
#include "bxr_shader.h"

typedef struct bxr_shader_bytecode_s_
{
  Uint8 *data;
  size_t size;
  SDL_GPUShaderFormat format;
} bxr_shader_bytecode_t_;

static Uint32 initialized_ = 0;
static bxr_context_t *context_;

void
bxr_shader_setup(bxr_context_t *context)
{
  BXR_ASSERT(initialized_ == 0);
  BXR_ASSERT(context != NULL);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;
}

void
bxr_shader_teardown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  initialized_ = 0;
  context_     = NULL;
}

// Return value must be freed as well as the data inside it.
static bxr_shader_bytecode_t_ *
_be_shader_load_bytecode(SDL_GPUDevice *device,
                         const char *name,
                         size_t *bytecode_size)
{
  BXR_ASSERT(device);
  BXR_ASSERT(name);

  bxr_shader_bytecode_t_ *shader_bytecode = NULL;
  BXR_NEW(shader_bytecode);
  if (!shader_bytecode) {
    return NULL;
  }

  // Determine the shader stage/type based on the file extension
  SDL_GPUShaderStage stage;

  if (SDL_strstr(name, ".vert") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_VERTEX;
  } else if (SDL_strstr(name, ".frag") != NULL) {
    stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  } else {
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
    BXR_LOG_ERROR(
        "GPU Device does not support any known shader formats (SPIR-V, MSL, "
        "DXIL, DXBC).");
    return NULL;
  }

  char shader_path[BXR_PATH_MAX_LENGTH];
  SDL_snprintf(shader_path, BXR_PATH_MAX_LENGTH, "%s%s", name, file_ext);

  shader_bytecode->data = bxr_asset_read(shader_path, &shader_bytecode->size);

  if (shader_bytecode->data == NULL) {
    BXR_FREE(shader_bytecode);
    BXR_LOG_ERROR("Failed to load shader bytecode from path: %s", shader_path);
    return NULL;
  }

  return shader_bytecode;
}

bxr_shader_t
bxr_shader_create(bxr_shader_desc_t *desc)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_shader_bytecode_t_ *vert_bytecode
      = _be_shader_load_bytecode(context_->gpu_device, desc->vert_name, NULL);
  if (!vert_bytecode) {
    return (bxr_shader_t){ .id = BXR_SHADER_INVALID_ID };
  }

  bxr_shader_bytecode_t_ *frag_bytecode
      = _be_shader_load_bytecode(context_->gpu_device, desc->frag_name, NULL);
  if (!frag_bytecode) {
    BXR_FREE(vert_bytecode->data);
    BXR_FREE(vert_bytecode);
    return (bxr_shader_t){ .id = BXR_SHADER_INVALID_ID };
  }

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

  bxr_shader_t shader = { .id = SDL_GPCreateShader(&shader_desc).id };

  BXR_FREE(vert_bytecode->data);
  BXR_FREE(vert_bytecode);

  BXR_FREE(frag_bytecode->data);
  BXR_FREE(frag_bytecode);

  return shader;
}

void
bxr_shader_destroy(bxr_shader_t shader)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  SDL_GPShader gp_shader = {
    .id = shader.id,
  };

  SDL_GPDestroyShader(gp_shader);
}
