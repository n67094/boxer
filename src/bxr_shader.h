/*
 * @file bxr_shader.h
 *
 * @brief Shader management system.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_SHADER_H_
#define BXR_SHADER_H_

#include <SDL3/SDL.h>

#include "bxr_config.h"
#include "bxr_context.h"

typedef struct bxr_shader_s
{
  Uint32 id;
} bxr_shader_t;

/**
 * @param name The name of the shader to load, without file extension (e.g.,
 * "my_shader.vert" and not "my_shader.vert.spv"). The name is critical for
 * determining the shader type.
 * @param num_samplers The number of samplers used.
 * @param num_storage_textures The number of storage textures.
 * @param num_storage_buffers The number of storage buffers used.
 * @param num_uniform_buffers The number of uniform buffers used.
 */
typedef struct bxr_shader_desc_s
{
  const char *name;
  Uint32 num_samplers;
  Uint32 num_storage_textures;
  Uint32 num_storage_buffers;
  Uint32 num_uniform_buffers;
} bxr_shader_desc_t;

typedef enum
{
  BXR_SHADER_TYPE_VERTEX,
  BXR_SHADER_TYPE_FRAGMENT,
} bxr_shader_type_e;

void bxr_shader_setup(bxr_context_t *context);

void bxr_shader_shutdown(void);

/**
 * @brief Load a shader.
 *
 * @details This function loads a shader from the "shaders/" directory,
 * appending the appropriate file extension based on the GPU driver used by the
 * device.
 */
bxr_shader_t bxr_shader_make(bxr_shader_desc_t *vertex_desc,
                             bxr_shader_desc_t *fragment_desc);

SDL_GPUShader *bxr_shader_get_vertex(bxr_shader_t shader);

SDL_GPUShader *bxr_shader_get_fragment(bxr_shader_t shader);

void bxr_shader_destroy(bxr_shader_t shader);

#endif // BXR_SHADER_H_
