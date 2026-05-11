#include <SDL3/SDL.h>

#include "bxr_error.h"

static bxr_erro_code_e last_error_ = BXR_ERROR_NONE;

bxr_erro_code_e
bxr_error_get(void)
{
  return last_error_;
}

void
bxr_error_set(bxr_erro_code_e error)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  SDL_LogError(
      SDL_LOG_CATEGORY_APPLICATION, "[%d] %s", error, bxr_error_get_msg(error));
  last_error_ = error;
}

void
bxr_set_error_info(bxr_erro_code_e error, const char *info)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
               "[%d] %s - INFO: %s",
               error,
               bxr_error_get_msg(error),
               info);
  last_error_ = error;
}

const char *
bxr_error_get_msg(bxr_erro_code_e error)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  switch (error) {
  case BXR_ERROR_NONE:
    return "No error.";
  case BXR_ERROR_OUT_OF_MEMORY:
    return "Out of memory.";
  case BXR_ERROR_FILE_NOT_FOUND:
    // bxr_asset.c errors
  case BXR_ERROR_ASSET_READ:
    return "Asset read error.";
  case BXR_ERROR_ASSET_WRITE:
    return "Asset write error.";
  case BXR_ERROR_ASSET_MOUNT:
    return "Asset mount error.";
  case BXR_ERROR_ASSET_UNMOUNT:
    return "Asset unmount error.";
  // bxr_shader.c errors
  case BXR_ERROR_SHADER_NAME:
    return "Shader name error.";
  case BXR_ERROR_SHADER_FORMAT:
    return "Shader format error.";
  case BXR_ERROR_SHADER_LOAD_BYTECODE:
    return "Shader load bytecode error.";
    // bxr_image.c errors
  case BXR_ERROR_IMAGE_READ:
    return "Image read error.";
  case BXR_ERROR_IMAGE_CREATE:
    return "Image create error.";
  case BXR_ERROR_IMAGE_CREATE_MEM:
    return "Image create from memory error.";
    // bxr_sampler.c errors
  case BXR_ERROR_SAMPLER_CREATE:
    return "Sampler create error.";
    // bxr_json.c errors
  case BXR_ERROR_JSON_READ:
    return "JSON read error.";
  case BXR_ERROR_JSON_WRITE:
    return "JSON write error.";
  default:
    return "Unknown error.";
  }
}
