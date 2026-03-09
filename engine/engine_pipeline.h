/**
 * @file engine_pipeline.h
 *
 * @brief Graphics pipeline management.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_PIPELINE_H_
#define ENGINE_PIPELINE_H_

#include <SDL3/SDL_gpu.h>

#include "engine_color.h"
#include "engine_context.h"
#include "engine_math.h"
#include "engine_shader.h"

typedef enum
{
  ENGINE_BLENDMODE_NONE = 0,            // SDL_BLENDMODE_NONE,
  ENGINE_BLENDMODE_BLEND,               // = SDL_BLENDMODE_BLEND,
  ENGINE_BLENDMODE_BLEND_PREMULTIPLIED, // = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
  ENGINE_BLENDMODE_ADD,                 // = SDL_BLENDMODE_ADD,
  ENGINE_BLENDMODE_ADD_PREMULTIPLIED,   // = SDL_BLENDMODE_ADD_PREMULTIPLIED,
  ENGINE_BLENDMODE_MOD,                 //= SDL_BLENDMODE_MOD,
  ENGINE_BLENDMODE_MUL,                 //= SDL_BLENDMODE_MUL,
  ENGINE_BLENDMODE_SIZE,                //= 7,
} engine_blendmode_e;

typedef enum
{
  ENGINE_PRIMITIVE_POINTS = 0,     // SDL_GPU_PRIMITIVETYPE_POINTLIST,
  ENGINE_PRIMITIVE_LINES,          // = SDL_GPU_PRIMITIVETYPE_LINELIST,
  ENGINE_PRIMITIVE_LINE_STRIP,     // = SDL_GPU_PRIMITIVETYPE_LINESTRIP,
  ENGINE_PRIMITIVE_TRIANGLES,      // = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
  ENGINE_PRIMITIVE_TRIANGLE_STRIP, // = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
  ENGINE_PRIMITIVE_SIZE,
} engine_primitive_e;

typedef struct engine_vertex_s
{
  engine_vec2_t position;
  engine_vec2_t texcoord;
  engine_color_t color;
} engine_vertex_t;

typedef struct engine_pipeline_s
{
  Uint32 id;
} engine_pipeline_t;

static const SDL_GPUPrimitiveType _engine_to_sdl_primitive_type[] = {
  SDL_GPU_PRIMITIVETYPE_POINTLIST,    // ENGINE_PRIMITIVE_POINTS
  SDL_GPU_PRIMITIVETYPE_LINELIST,     // ENGINE_PRIMITIVE_LINES
  SDL_GPU_PRIMITIVETYPE_LINESTRIP,    // ENGINE_PRIMITIVE_LINE_STRIP
  SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, // ENGINE_PRIMITIVE_TRIANGLES
  SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP // ENGINE_PRIMITIVE_TRIANGLE_STRIP
};

ENGINE_INLINE SDL_GPUPrimitiveType
engine_sdl_primitive_type(engine_primitive_e primitive_type)
{
  return _engine_to_sdl_primitive_type[primitive_type];
}

static const SDL_BlendMode _engine_to_sdl_blend_mode[] = {
  SDL_BLENDMODE_NONE,                // ENGINE_BLENDMODE_NONE
  SDL_BLENDMODE_BLEND,               // ENGINE_BLENDMODE_BLEND
  SDL_BLENDMODE_BLEND_PREMULTIPLIED, // ENGINE_BLENDMODE_BLEND_PREMULTIPLIED
  SDL_BLENDMODE_ADD,                 // ENGINE_BLENDMODE_ADD
  SDL_BLENDMODE_ADD_PREMULTIPLIED,   // ENGINE_BLENDMODE_ADD_PREMULTIPLIED
  SDL_BLENDMODE_MOD,                 // ENGINE_BLENDMODE_MOD
  SDL_BLENDMODE_MUL                  // ENGINE_BLENDMODE_MUL
};

ENGINE_INLINE SDL_BlendMode
engine_sdl_blend_mode(engine_blendmode_e blend_mode)
{
  return _engine_to_sdl_blend_mode[blend_mode];
}

void engine_pipeline_setup(engine_context_t *context);

void engine_pipeline_shutdown(void);

engine_pipeline_t engine_pipeline_make(engine_shader_t shader,
                                       engine_primitive_e primitive_type,
                                       engine_blendmode_e blend_mode);

SDL_GPUGraphicsPipeline *engine_pipeline_get(engine_pipeline_t pipeline);

void engine_pipeline_destroy(engine_pipeline_t pipeline);

#endif // ENGINE_PIPELINE_H_
