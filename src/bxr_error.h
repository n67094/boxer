/**
 * File `bxr_error.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Error handling system
 */

#ifndef BXR_ERROR_H_
#define BXR_ERROR_H_

typedef enum
{
  BXR_ERROR_NONE = 0,
  BXR_ERROR_FILE_NOT_FOUND,
  // bxr_shader.c errors
  BXR_ERROR_SHADER_NAME,
  BXR_ERROR_SHADER_FORMAT,
  BXR_ERROR_SHADER_LOAD_BYTECODE,
  // bxr_image.c errors
  BXR_ERROR_IMAGE_FILE,
  BXR_ERROR_IMAGE_MAKE,
  BXR_ERROR_IMAGE_MAKE_MEM,
  // bxr_sampler.c errors
  BXR_ERROR_SAMPLER_MAKE,
  // bxr_io.c errors
  BXR_ERROR_IO_READ,
  BXR_ERROR_IO_WRITE,
  // bxr_json.c errors remove those error don't care
  BXR_ERROR_JSON_READ,
  BXR_ERROR_JSON_WRITE,
  BXR_ERROR_SIZE,
} bxr_erro_code_e;

/**
 * ## Internal API
 */

/**
 * Set the last error code.
 *
 * `error` is the error code to set.
 */
void bxr_set_error(bxr_erro_code_e error);

/**
 * ## Public API
 */

/**
 * Get the last error code.
 *
 * `return` the last error code.
 */
bxr_erro_code_e bxr_get_error(void);

/**
 * Get the error message corresponding to the given error code.
 *
 * `error` is the error code to get the message for.
 *
 * `return` the error message corresponding to the given error code.
 */
const char *bxr_get_error_msg(bxr_erro_code_e error);

#endif // BXR_ERROR_H_
