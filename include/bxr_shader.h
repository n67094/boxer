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
 *
 */

#ifndef BXR_SHADER_H_
#define BXR_SHADER_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_sdl_gp.h"

#define BXR_SHADER_INVALID_ID SDL_GP_INVALID_ID

typedef enum
{
  BXR_SHADER_STAGE_VERTEX   = SDL_GPU_SHADERSTAGE_VERTEX,
  BXR_SHADER_STAGE_FRAGMENT = SDL_GPU_SHADERSTAGE_FRAGMENT,
  BXR_SHADER_STAGE_SIZE     = 2,
} bxr_shader_stage_e;

typedef struct bxr_shader_s
{
  size_t id;
} bxr_shader_t;

typedef struct bxr_shader_desc_s
{
  const char *name;
  const char *entrypoint;
  bxr_shader_stage_e stage;
  Uint32 num_samplers;
  Uint32 num_storage_textures;
  Uint32 num_storage_buffers;
  Uint32 num_uniform_buffers;
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
void bxr_shader_teardown(void);

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
BXR_API bxr_shader_t bxr_shader_create(const bxr_shader_desc_t *desc);

/**
 * Returns the SDL vertex shader associated with the given boxer shader.
 */
BXR_API SDL_GPUShader *bxr_get_sdl_shader(bxr_shader_t shader);

/**
 * Destroys a shader, freeing any resources associated with it.
 *
 * `shader` is the shader to destroy.
 */
BXR_API void bxr_shader_destroy(bxr_shader_t shader);

#endif // BXR_SHADER_H_
