#include <SDL3/SDL.h>

#include "engine_error.h"

static engine_erro_code_e _last_error = ENGINE_ERROR_NONE;

engine_erro_code_e
engine_get_error(void)
{
  return _last_error;
}

void
engine_set_error(engine_erro_code_e error)
{
  SDL_assert(error >= ENGINE_ERROR_NONE && error < ENGINE_ERROR_SIZE);

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
               "[%d] %s",
               error,
               engine_get_error_msg(error));
  _last_error = error;
}

void
engine_set_error_info(engine_erro_code_e error, const char *info)
{
  SDL_assert(error >= ENGINE_ERROR_NONE && error < ENGINE_ERROR_SIZE);

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
               "[%d] %s - INFO: %s",
               error,
               engine_get_error_msg(error),
               info);
  _last_error = error;
}

const char *
engine_get_error_msg(engine_erro_code_e error)
{
  SDL_assert(error >= ENGINE_ERROR_NONE && error < ENGINE_ERROR_SIZE);

  switch (error) {
  case ENGINE_ERROR_NONE:
    return "No error.";
  case ENGINE_ERROR_FILE_NOT_FOUND:
    return "File not found.";
  // engine_shader.c errors
  case ENGINE_ERROR_SHADER_LOAD:
    return "Shader load error.";
  case ENGINE_ERROR_SHADER_LOAD_BYTECODE:
    return "Shader load bytecode error.";
  // engine_image.c errors
  case ENGINE_ERROR_IMAGE_SETUP:
    return "Image setup error.";
  case ENGINE_ERROR_IMAGE_LOAD:
    return "Image load error.";
  case ENGINE_ERROR_IMAGE_MAKE_MEM:
    return "Image from memory error.";
  // engine_painter.c errors
  case ENGINE_ERROR_PAINTER_SETUP:
    return "Painter setup error.";
  case ENGINE_ERROR_PAINTER_VERTICES_FULL:
    return "Painter vertices buffer full.";
  case ENGINE_ERROR_PAINTER_COMMANDS_FULL:
    return "Painter commands buffer full.";
  case ENGINE_ERROR_PAINTER_FLUSH:
    return "Painter flush error.";
  // engine_io.c errors
  case ENGINE_ERROR_IO_READ:
    return "IO read error.";
  case ENGINE_ERROR_IO_WRITE:
    return "IO write error.";
    // engine_json.c errors
  case ENGINE_ERROR_JSON_READ:
    return "JSON read error.";
  case ENGINE_ERROR_JSON_WRITE:
    return "JSON write error.";
  default:
    return "Unknown error.";
  }
}
