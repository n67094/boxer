/*
 * file bxr_shader.h
 *
 * copyright Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Shader management system.
 *
 */

#ifndef BXR_SHADER_H_
#define BXR_SHADER_H_

#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include "bxr_context.h"

typedef SDL_GPShader bxr_shader_t;

typedef struct bxr_shader_desc_s
{
  const char *vert_name;
  const char *vert_entrypoint;
  Uint32 vert_num_samplers;
  Uint32 vert_num_storage_textures;
  Uint32 vert_num_storage_buffers;
  Uint32 vert_num_uniform_buffers;

  const char *frag_name;
  const char *frag_entrypoint;
  Uint32 frag_num_samplers;
  Uint32 frag_num_storage_textures;
  Uint32 frag_num_storage_buffers;
  Uint32 frag_num_uniform_buffers;
} bxr_shader_desc_t;

/**
 * ## Internal API
 */

/**
 * Initializes the shader system. Must be called before using any other shader
 * functions.
 *
 * `context` is the bxr context to use for shader operations.
 */
void bxr_shader_setup(bxr_context_t *context);

/**
 * Shuts down the shader system.
 */
void bxr_shader_shutdown(void);

/**
 * ## Public API
 */

/**
 * Creates a shader from the given description.
 *
 * `desc` is the description of the shader to create.
 *
 * `return` the created shader, or an invalid shader if the shader could not
 * be created.
 */
bxr_shader_t bxr_shader_make(bxr_shader_desc_t *desc);

/**
 * Destroys a shader, freeing any resources associated with it.
 *
 * `shader` is the shader to destroy.
 */
void bxr_shader_destroy(bxr_shader_t shader);

#endif // BXR_SHADER_H_
