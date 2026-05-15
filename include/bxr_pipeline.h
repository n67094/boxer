/**
 * File `bxr_pipeline.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Graphics Pipeline Management
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include "bxr_shader.h"

typedef struct bxr_pipeline_s
{
  size_t id;
} bxr_pipeline_t;

typedef enum
{
  BXR_BLENDMODE_NONE                = SDL_BLENDMODE_NONE,
  BXR_BLENDMODE_BLEND               = SDL_BLENDMODE_BLEND,
  BXR_BLENDMODE_BLEND_PREMULTIPLIED = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
  BXR_BLENDMODE_ADD                 = SDL_BLENDMODE_ADD,
  BXR_BLENDMODE_ADD_PREMULTIPLIED   = SDL_BLENDMODE_ADD_PREMULTIPLIED,
  BXR_BLENDMODE_MOD                 = SDL_BLENDMODE_MOD,
  BXR_BLENDMODE_MUL                 = SDL_BLENDMODE_MUL,
  BXR_BLENDMODE_SIZE                = 7,
} bxr_blendmode_e;

typedef enum
{
  BXR_PRIMITIVE_TRIANGLES      = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
  BXR_PRIMITIVE_TRIANGLE_STRIP = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
  BXR_PRIMITIVE_LINES          = SDL_GPU_PRIMITIVETYPE_LINELIST,
  BXR_PRIMITIVE_LINE_STRIP     = SDL_GPU_PRIMITIVETYPE_LINESTRIP,
  BXR_PRIMITIVE_POINTS         = SDL_GPU_PRIMITIVETYPE_POINTLIST,
  BXR_PRIMITIVE_SIZE           = 5,
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
bxr_pipeline_t bxr_pipeline_create(bxr_shader_t shader,
                                   bxr_primitive_e primitive_type,
                                   bxr_blendmode_e blend_mode);

/**
 * Destroy a graphics pipeline.
 *
 * `pipeline` is the pipeline to destroy.
 */
void bxr_pipeline_destroy(bxr_pipeline_t pipeline);

#endif // BXR_PIPELINE_H_
