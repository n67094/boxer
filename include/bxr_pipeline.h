/**
 * File `bxr_pipeline.h`.
 *
 * Custom graphics pipelines for the painter.
 *
 * Typical usage: create shaders with `bxr_shader_create()`, combine them with
 * `bxr_pipeline_create()`, select the pipeline with
 * `bxr_painter_set_pipeline()`, and release it with `bxr_pipeline_destroy()`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include "bxr_sdl_gp.h"
#include "bxr_shader.h"

#define BXR_PIPELINE_INVALID_ID SDL_GP_INVALID_ID

typedef struct bxr_pipeline_s
{
  size_t id;
} bxr_pipeline_t;

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
  BXR_PRIMITIVE_SIZE           = SDL_GP_PRIMITIVE_SIZE
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
 * Destroy a graphics pipeline and release its GPU resources.
 *
 * `pipeline` a pipeline from `bxr_pipeline_create()`. It must not be in use
 * by the painter for the current frame.
 */
BXR_API void bxr_pipeline_destroy(bxr_pipeline_t pipeline);

#endif // BXR_PIPELINE_H_
