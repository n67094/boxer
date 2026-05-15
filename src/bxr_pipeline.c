#include <SDL_gp.h>

#include "bxr_pipeline.h"

bxr_pipeline_t
bxr_pipeline_create(bxr_shader_t shader,
                    bxr_primitive_e primitive_type,
                    bxr_blendmode_e blend_mode)
{
  SDL_GPShader gp_shader = {
    .id = shader.id,
  };
  SDL_GPPrimitiveType gp_primitive_type = (SDL_GPPrimitiveType)primitive_type;
  SDL_GPBlendMode gp_blend_mode         = (SDL_GPBlendMode)blend_mode;

  return (bxr_pipeline_t){
    .id = SDL_GPCreatePipeline(gp_shader, gp_primitive_type, gp_blend_mode).id
  };
}

void
bxr_pipeline_destroy(bxr_pipeline_t pipeline)
{
  SDL_GPDestroyPipeline((SDL_GPPipeline){ .id = pipeline.id });
}
