#include "bxr_pipeline.h"
#include "bxr_error.h"
#include "bxr_sdl_gp.h"
#include "bxr_shader.h"

bxr_pipeline_t
bxr_pipeline_create(bxr_shader_t shader_vert,
                    bxr_shader_t shader_frag,
                    bxr_primitive_e primitive_type,
                    bxr_blendmode_e blend_mode)
{
  SDL_GPShader gp_shader_vert = {
    .id = shader_vert.id,
  };
  SDL_GPShader gp_shader_frag = {
    .id = shader_frag.id,
  };
  SDL_GPPrimitiveType gp_primitive_type = (SDL_GPPrimitiveType)primitive_type;
  SDL_GPBlendMode gp_blend_mode         = (SDL_GPBlendMode)blend_mode;

  bxr_pipeline_t pipeline = { .id = SDL_GPCreatePipeline(gp_shader_vert,
                                                         gp_shader_frag,
                                                         gp_primitive_type,
                                                         gp_blend_mode)
                                        .id };

  if (pipeline.id == BXR_PIPELINE_INVALID_ID) {
    bxr_set_error("Failed to create pipeline (error: %s)",
                  SDL_GPGetErrorMessage(SDL_GPGetLastError()));
  }

  return pipeline;
}

bool
bxr_pipeline_is_valid(bxr_pipeline_t pipeline)
{
  return pipeline.id != BXR_PIPELINE_INVALID_ID;
}

void
bxr_pipeline_destroy(bxr_pipeline_t pipeline)
{
  SDL_GPDestroyPipeline((SDL_GPPipeline){ .id = pipeline.id });
}
