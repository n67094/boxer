#include <SDL3/SDL.h>

#include "bxr_sdl_gp.h"

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
_bxr_shader_load_bytecode(SDL_GPUDevice *device,
                          const char *name,
                          bxr_shader_stage_e stage)
{
  BXR_ASSERT(device);
  BXR_ASSERT(name);

  bxr_shader_bytecode_t_ *shader_bytecode = NULL;
  BXR_NEW(shader_bytecode);
  if (!shader_bytecode) {
    return NULL;
  }

  if (SDL_strstr(name, ".vert") != NULL) {
    BXR_ASSERT(stage == BXR_SHADER_STAGE_VERTEX);
  } else if (SDL_strstr(name, ".frag") != NULL) {
    BXR_ASSERT(stage == BXR_SHADER_STAGE_FRAGMENT);
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
    BXR_FREE(shader_bytecode);
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
bxr_shader_create(const bxr_shader_desc_t *desc)
{
  BXR_ASSERT(desc != NULL);
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  bxr_shader_bytecode_t_ *bytecode = _bxr_shader_load_bytecode(
      context_->gpu_device, desc->name, desc->stage);
  if (!bytecode) {
    BXR_LOG_ERROR("Failed to load shader bytecode for shader: %s", desc->name);
    return (bxr_shader_t){ .id = BXR_SHADER_INVALID_ID };
  }

  SDL_GPShaderDesc shader_desc = {
    // Vertex shader description
    .code_size            = bytecode->size,
    .code                 = bytecode->data,
    .entrypoint           = desc->entrypoint,
    .format               = bytecode->format,
    .stage                = (SDL_GPUShaderStage)desc->stage,
    .num_samplers         = desc->num_samplers,
    .num_storage_textures = desc->num_storage_textures,
    .num_storage_buffers  = desc->num_storage_buffers,
    .num_uniform_buffers  = desc->num_uniform_buffers,
  };

  bxr_shader_t shader = { .id = SDL_GPCreateShader(&shader_desc).id };

  if (shader.id == BXR_SHADER_INVALID_ID) {
    if (bytecode->format == SDL_GPU_SHADERFORMAT_SPIRV) {
      BXR_LOG_ERROR("Failed to create shader from SPIR-V bytecode: %s",
                    desc->name);
    } else if (bytecode->format == SDL_GPU_SHADERFORMAT_MSL) {
      BXR_LOG_ERROR("Failed to create shader from MSL bytecode: %s",
                    desc->name);
    } else if (bytecode->format == SDL_GPU_SHADERFORMAT_DXIL) {
      BXR_LOG_ERROR("Failed to create shader from DXIL bytecode: %s",
                    desc->name);
    } else if (bytecode->format == SDL_GPU_SHADERFORMAT_DXBC) {
      BXR_LOG_ERROR("Failed to create shader from DXBC bytecode: %s",
                    desc->name);
    } else {
      BXR_LOG_ERROR("Failed to create shader from unknown format bytecode: %s",
                    desc->name);
    }
  }

  BXR_FREE(bytecode->data);
  BXR_FREE(bytecode);

  return shader;
}

SDL_GPUShader *
bxr_shader_get_sdl(bxr_shader_t shader)
{
  return SDL_GPGetGPUShader((SDL_GPShader){ .id = shader.id });
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
