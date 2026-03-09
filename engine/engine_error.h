/**
 * @file engine_error.h
 *
 * @brief The engine error handling system.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_ERROR_H_
#define ENGINE_ERROR_H_

typedef enum
{
  ENGINE_ERROR_NONE = 0,
  ENGINE_ERROR_FILE_NOT_FOUND,
  // engine_shader.c errors
  ENGINE_ERROR_SHADER_LOAD,
  ENGINE_ERROR_SHADER_MAKE,
  ENGINE_ERROR_SHADER_LOAD_BYTECODE,
  // engine_image.c errors
  ENGINE_ERROR_IMAGE_SETUP,
  ENGINE_ERROR_IMAGE_LOAD,
  ENGINE_ERROR_IMAGE_MAKE,
  ENGINE_ERROR_IMAGE_MAKE_MEM,
  // engine_pipeline.c errors
  ENGINE_ERROR_PIPELINE_MAKE,
  // engine_painter.c errors
  ENGINE_ERROR_PAINTER_SETUP,
  ENGINE_ERROR_PAINTER_VERTICES_FULL,
  ENGINE_ERROR_PAINTER_COMMANDS_FULL,
  ENGINE_ERROR_PAINTER_UNIFORMS_FULL,
  ENGINE_ERROR_PAINTER_FLUSH,
  // engine_io.c errors
  ENGINE_ERROR_IO_READ,
  ENGINE_ERROR_IO_WRITE,
  // engine_json.c errors remove those error don't care
  ENGINE_ERROR_JSON_READ,
  ENGINE_ERROR_JSON_WRITE,
  ENGINE_ERROR_SIZE,
} engine_erro_code_e;

engine_erro_code_e engine_get_error(void);

void engine_set_error(engine_erro_code_e error);

void engine_set_error_info(engine_erro_code_e error, const char *info);

const char *engine_get_error_msg(engine_erro_code_e error);

#endif // ENGINE_ERROR_H_
