/**
 * File `bxr_pipeline.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Custom Graphics Pipelines for the Painter
 *
 * Typical usage: create shaders with `bxr_shader_create()`, combine them with
 * `bxr_pipeline_create()`, select the pipeline with
 * `bxr_painter_set_pipeline()`, and release it with `bxr_pipeline_destroy()`.
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include "bxr_shader.h"

#define BXR_PIPELINE_INVALID_ID 0

typedef struct bxr_pipeline_s
{
  size_t id;
} bxr_pipeline_t;

typedef enum
{
  BXR_BLENDMODE_NONE = 0,
  BXR_BLENDMODE_BLEND,
  BXR_BLENDMODE_BLEND_PREMULTIPLIED,
  BXR_BLENDMODE_ADD,
  BXR_BLENDMODE_ADD_PREMULTIPLIED,
  BXR_BLENDMODE_MOD,
  BXR_BLENDMODE_MUL,
  BXR_BLENDMODE_SIZE,
} bxr_blendmode_e;

typedef enum
{
  BXR_PRIMITIVE_TRIANGLES      = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
  BXR_PRIMITIVE_TRIANGLE_STRIP = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
  BXR_PRIMITIVE_LINES          = SDL_GPU_PRIMITIVETYPE_LINELIST,
  BXR_PRIMITIVE_LINE_STRIP     = SDL_GPU_PRIMITIVETYPE_LINESTRIP,
  BXR_PRIMITIVE_POINTS         = SDL_GPU_PRIMITIVETYPE_POINTLIST,
  BXR_PRIMITIVE_SIZE           = 5
} bxr_primitive_e;

/**
 * ## Public API
 */

/**
 * Create a graphics pipeline from a vertex and a fragment shader.
 *
 * `shader_vert` a vertex shader from `bxr_shader_create()`.
 * `shader_frag` a fragment shader from `bxr_shader_create()`.
 * `primitive_type` how vertices are assembled into primitives.
 * `blend_mode` how fragments are blended with the render target.
 *
 * `return` the new pipeline, or a pipeline with `id` 0 on failure.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller owns the pipeline and must release it with
 * `bxr_pipeline_destroy()`. The shaders can be destroyed once the pipeline is
 * created.
 */
BXR_API bxr_pipeline_t bxr_pipeline_create(bxr_shader_t shader_vert,
                                           bxr_shader_t shader_frag,
                                           bxr_primitive_e primitive_type,
                                           bxr_blendmode_e blend_mode);

/**
 * Check if a graphics pipeline handle is valid.
 *
 * `pipeline` a pipeline from `bxr_pipeline_create()`.
 *
 * `return` true if the pipeline is valid, false otherwise.
 */
BXR_API bool bxr_pipeline_is_valid(bxr_pipeline_t pipeline);

/**
 * Destroy a graphics pipeline and release its GPU resources.
 *
 * `pipeline` a pipeline from `bxr_pipeline_create()`. It must not be in use
 * by the painter for the current frame.
 */
BXR_API void bxr_pipeline_destroy(bxr_pipeline_t pipeline);

#endif // BXR_PIPELINE_H_
