/**
 * @file bxr_error.h
 *
 * @brief The bxr error handling system.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_ERROR_H_
#define BXR_ERROR_H_

typedef enum
{
  BXR_ERROR_NONE = 0,
  BXR_ERROR_FILE_NOT_FOUND,
  // bxr_shader.c errors
  BXR_ERROR_SHADER_LOAD,
  BXR_ERROR_SHADER_MAKE,
  BXR_ERROR_SHADER_LOAD_BYTECODE,
  // bxr_image.c errors
  BXR_ERROR_IMAGE_SETUP,
  BXR_ERROR_IMAGE_LOAD,
  BXR_ERROR_IMAGE_MAKE,
  BXR_ERROR_IMAGE_MAKE_MEM,
  // bxr_pipeline.c errors
  BXR_ERROR_PIPELINE_MAKE,
  // bxr_painter.c errors
  BXR_ERROR_PAINTER_SETUP,
  BXR_ERROR_PAINTER_VERTICES_FULL,
  BXR_ERROR_PAINTER_COMMANDS_FULL,
  BXR_ERROR_PAINTER_UNIFORMS_FULL,
  BXR_ERROR_PAINTER_FLUSH,
  // bxr_io.c errors
  BXR_ERROR_IO_READ,
  BXR_ERROR_IO_WRITE,
  // bxr_json.c errors remove those error don't care
  BXR_ERROR_JSON_READ,
  BXR_ERROR_JSON_WRITE,
  BXR_ERROR_SIZE,
} bxr_erro_code_e;

bxr_erro_code_e bxr_get_error(void);

void bxr_set_error(bxr_erro_code_e error);

void bxr_set_error_info(bxr_erro_code_e error, const char *info);

const char *bxr_get_error_msg(bxr_erro_code_e error);

#endif // BXR_ERROR_H_
