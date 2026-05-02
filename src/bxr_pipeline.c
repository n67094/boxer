#include "bxr_pipeline.h"

bxr_pipeline_t
bxr_pipeline_create(bxr_shader_t shader,
                    bxr_primitive_e primitive_type,
                    bxr_blendmode_e blend_mode)
{
  return (bxr_pipeline_t)SDL_GPCreatePipeline(
      (SDL_GPShader)shader,
      (SDL_GPPrimitiveType)primitive_type,
      (SDL_GPBlendMode)blend_mode);
};

void
bxr_pipeline_destroy(bxr_pipeline_t pipeline)
{
  SDL_GPDestroyPipeline((SDL_GPPipeline)pipeline);
};
