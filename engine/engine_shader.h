/*
 * @file engine_shader.h
 *
 * @brief Shader management system.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_SHADER_H_
#define ENGINE_SHADER_H_

#include <SDL3/SDL.h>

#include "engine_config.h"
#include "engine_context.h"

typedef struct engine_shader_s
{
  Uint32 id;
} engine_shader_t;

/**
 * @param name The name of the shader to load, without file extension (e.g.,
 * "my_shader.vert" and not "my_shader.vert.spv"). The name is critical for
 * determining the shader type.
 * @param num_samplers The number of samplers used.
 * @param num_storage_textures The number of storage textures.
 * @param num_storage_buffers The number of storage buffers used.
 * @param num_uniform_buffers The number of uniform buffers used.
 */
typedef struct engine_shader_desc_s
{
  const char *name;
  Uint32 num_samplers;
  Uint32 num_storage_textures;
  Uint32 num_storage_buffers;
  Uint32 num_uniform_buffers;
} engine_shader_desc_t;

typedef enum
{
  ENGINE_SHADER_TYPE_VERTEX,
  ENGINE_SHADER_TYPE_FRAGMENT,
} engine_shader_type_e;

void engine_shader_setup(engine_context_t *context);

void engine_shader_shutdown(void);

/**
 * @brief Load a shader.
 *
 * @details This function loads a shader from the "shaders/" directory,
 * appending the appropriate file extension based on the GPU driver used by the
 * device.
 */
engine_shader_t engine_shader_make(engine_shader_desc_t *vertex_desc,
                                   engine_shader_desc_t *fragment_desc);

SDL_GPUShader *engine_shader_get_vertex(engine_shader_t shader);

SDL_GPUShader *engine_shader_get_fragment(engine_shader_t shader);

void engine_shader_destroy(engine_shader_t shader);

#endif // ENGINE_SHADER_H_
