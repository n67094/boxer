#include <SDL3/SDL.h>

#include "bxr_error.h"

static bxr_erro_code_e _last_error = BXR_ERROR_NONE;

bxr_erro_code_e
bxr_get_error(void)
{
  return _last_error;
}

void
bxr_set_error(bxr_erro_code_e error)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  SDL_LogError(
      SDL_LOG_CATEGORY_APPLICATION, "[%d] %s", error, bxr_get_error_msg(error));
  _last_error = error;
}

void
bxr_set_error_info(bxr_erro_code_e error, const char *info)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
               "[%d] %s - INFO: %s",
               error,
               bxr_get_error_msg(error),
               info);
  _last_error = error;
}

const char *
bxr_get_error_msg(bxr_erro_code_e error)
{
  SDL_assert(error >= BXR_ERROR_NONE && error < BXR_ERROR_SIZE);

  switch (error) {
  case BXR_ERROR_NONE:
    return "No error.";
  case BXR_ERROR_FILE_NOT_FOUND:
    return "File not found.";
  // bxr_shader.c errors
  case BXR_ERROR_SHADER_NAME:
    return "Shader name error.";
  case BXR_ERROR_SHADER_FORMAT:
    return "Shader format error.";
  case BXR_ERROR_SHADER_LOAD_BYTECODE:
    return "Shader load bytecode error.";
    // bxr_image.c errors
  case BXR_ERROR_IMAGE_FILE:
    return "Image open file error.";
  case BXR_ERROR_IMAGE_MAKE:
    return "Image make error.";
  case BXR_ERROR_IMAGE_MAKE_MEM:
    return "Image make from memory error.";
    // bxr_sampler.c errors
  case BXR_ERROR_SAMPLER_MAKE:
    return "Sampler make error.";
    // bxr_io.c errors
  case BXR_ERROR_IO_READ:
    return "IO read error.";
  case BXR_ERROR_IO_WRITE:
    return "IO write error.";
    // bxr_json.c errors
  case BXR_ERROR_JSON_READ:
    return "JSON read error.";
  case BXR_ERROR_JSON_WRITE:
    return "JSON write error.";
  default:
    return "Unknown error.";
  }
}
