/**
 * File `bxr_pipeline.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Pipelines
 *
 * Typical usage:
 *
 * ```cpp
 * bxr_shader_t vert = bxr_shader_create(&(bxr_shader_desc_t){
 *   .name       = "shaders/sprite.vert",
 *   .entrypoint = "main",
 *   .stage      = BXR_SHADER_STAGE_VERTEX,
 * });
 * bxr_shader_t frag = bxr_shader_create(&(bxr_shader_desc_t){
 *   .name         = "shaders/sprite.frag",
 *   .entrypoint   = "main",
 *   .stage        = BXR_SHADER_STAGE_FRAGMENT,
 *   .num_samplers = 1,
 * });
 *
 * _pipeline = bxr_pipeline_create(vert, frag, BXR_PRIMITIVE_TRIANGLES,
 *                                 BXR_BLENDMODE_BLEND);
 * bxr_shader_destroy(vert); // no longer needed once the pipeline exists
 * bxr_shader_destroy(frag);
 *
 * _sampler = bxr_sampler_create(BXR_SAMPLER_POINT_CLAMP);
 *
 * ...
 *
 * // in bxr_game_render()
 * bxr_painter_set_pipeline(_pipeline);
 * bxr_painter_set_sampler(0, &_sampler);
 * ... draw ...
 * bxr_painter_reset_sampler(0);
 * bxr_painter_reset_pipeline();
 *
 * ...
 *
 * bxr_sampler_destroy(&_sampler);
 * bxr_pipeline_destroy(_pipeline);
 * ```
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include "bxr_shader.h"

/** @self */
#define BXR_PIPELINE_INVALID_ID 0

/**
 * ## Enums
 */

/**
 * Blend mode for the pipeline.
 */
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

/**
 * Primitive type for the pipeline.
 */
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
 * ## Typedefs
 */

/**
 * Handle to a graphics pipeline.
 */
typedef struct bxr_pipeline_s
{
  size_t id;
} bxr_pipeline_t;

/**
 * ## API
 */

/**
 * Create a graphics pipeline from a vertex and a fragment shader.
 *
 * `shader_vert` a vertex shader from `bxr_shader_create()`.
 *
 * `shader_frag` a fragment shader from `bxr_shader_create()`.
 *
 * `primitive_type` how vertices are assembled into primitives.
 *
 * `blend_mode` how fragments are blended with the render target.
 *
 * `return` the new pipeline, or a pipeline with `id` 0 on failure.
 *
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
