/**
 * File `bxr_shader.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Shader Management System.
 *
 * Shader are loaded from mounted directories using `bxr_asset.h` API.
 *
 * Shader should be name like this `<name>.<stage>.<format>`, where:
 *
 * - `<name>` is the name of the shader (e.g. "basic", "sprite", etc.).
 * - `<stage>` is the shader stage (e.g. "vert" for vertex shader, "frag" for
 * fragment shader).
 * - `<format>` is the shader bytecode format (e.g. "spv" for SPIR-V, "msl" for
 * Metal Shading Language, etc.).
 */

#ifndef BXR_SHADER_H_
#define BXR_SHADER_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"

#define BXR_SHADER_INVALID_ID SDL_GP_INVALID_ID

typedef struct bxr_shader_s
{
  size_t id;
} bxr_shader_t;

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
 *
 * The caller is responsible for destroying the returned shader using
 * `bxr_shader_destroy` when it is no longer needed.
 */
bxr_shader_t bxr_shader_create(bxr_shader_desc_t *desc);

/**
 * Destroys a shader, freeing any resources associated with it.
 *
 * `shader` is the shader to destroy.
 */
void bxr_shader_destroy(bxr_shader_t shader);

#endif // BXR_SHADER_H_
