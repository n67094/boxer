/**
 * @file bxr_pipeline.h
 *
 * @brief Graphics pipeline management and utilities.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_PIPELINE_H_
#define BXR_PIPELINE_H_

#include <SDL3/SDL_gpu.h>

#include "bxr_color.h"
#include "bxr_context.h"
#include "bxr_math.h"

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

typedef struct bxr_vertex_s
{
  bxr_vec2_t position;
  bxr_vec2_t texcoord;
  bxr_color_t color;
} bxr_vertex_t;

typedef struct bxr_pipeline_s
{
  Uint32 id;
} bxr_pipeline_t;

void bxr_pipeline_setup(bxr_context_t *context);

void bxr_pipeline_shutdown(void);

/**
 * @brief Attach an SDL pipeline to an bxr pipeline. Usefull to use custom
 * pipelines with the painter.
 */
bxr_pipeline_t bxr_pipeline_attach(SDL_GPUGraphicsPipeline *pipeline);

SDL_GPUGraphicsPipeline *bxr_pipeline_get(bxr_pipeline_t pipeline);

void bxr_pipeline_destroy(bxr_pipeline_t pipeline);

#endif // BXR_PIPELINE_H_
