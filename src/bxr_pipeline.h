/**
 * File `bxr_pipeline.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Graphics Pipeline Management
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include <SDL_gp.h>

#include "bxr_shader.h"

typedef SDL_GPPipeline bxr_pipeline_t;

typedef enum
{
  BXR_BLENDMODE_NONE                = SDL_GP_BLENDMODE_NONE,
  BXR_BLENDMODE_BLEND               = SDL_GP_BLENDMODE_BLEND,
  BXR_BLENDMODE_BLEND_PREMULTIPLIED = SDL_GP_BLENDMODE_BLEND_PREMULTIPLIED,
  BXR_BLENDMODE_ADD                 = SDL_GP_BLENDMODE_ADD,
  BXR_BLENDMODE_ADD_PREMULTIPLIED   = SDL_GP_BLENDMODE_ADD_PREMULTIPLIED,
  BXR_BLENDMODE_MOD                 = SDL_GP_BLENDMODE_MOD,
  BXR_BLENDMODE_MUL                 = SDL_GP_BLENDMODE_MUL,
  BXR_BLENDMODE_SIZE                = SDL_GP_BLENDMODE_SIZE,
} bxr_blendmode_e;

typedef enum
{
  BXR_PRIMITIVE_TRIANGLES      = SDL_GP_PRIMITIVE_TRIANGLES,
  BXR_PRIMITIVE_TRIANGLE_STRIP = SDL_GP_PRIMITIVE_TRIANGLE_STRIP,
  BXR_PRIMITIVE_LINES          = SDL_GP_PRIMITIVE_LINES,
  BXR_PRIMITIVE_LINE_STRIP     = SDL_GP_PRIMITIVE_LINE_STRIP,
  BXR_PRIMITIVE_POINTS         = SDL_GP_PRIMITIVE_POINTS,
  BXR_PRIMITIVE_SIZE           = SDL_GP_PRIMITIVE_SIZE,
} bxr_primitive_e;

/**
 * ## Public API
 */

/**
 * Create a graphics pipeline with the given shader, primitive type, and blend
 * mode.
 *
 * `shader` is the shader to use for the pipeline.
 *
 * `primitive_type` is the type of primitives to render with the pipeline.
 *
 * `blend_mode` is the blend mode to use for rendering with the pipeline.
 *
 * `return` the created pipeline, or an invalid pipeline if the pipeline could
 * not be created.
 *
 * The caller is responsible for destroying the returned pipeline using
 * `bxr_pipeline_destroy` when it is no longer needed.
 */
bxr_pipeline_t bxr_pipeline_make(bxr_shader_t shader,
                                 bxr_primitive_e primitive_type,
                                 bxr_blendmode_e blend_mode);

/**
 * Destroy a graphics pipeline.
 *
 * `pipeline` is the pipeline to destroy.
 */
void bxr_pipeline_destroy(bxr_pipeline_t pipeline);

#endif // BXR_PIPELINE_H_
