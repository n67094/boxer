/**
 * @file engine_image.h
 *
 * @brief Image loading and management.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_IMAGE_H_
#define ENGINE_IMAGE_H_

#include <SDL3/SDL_gpu.h>

#include "engine_context.h"

typedef enum
{
  ENGINE_FONT_ALIGN_LEFT,
  ENGINE_FONT_ALIGN_CENTER,
  ENGINE_FONT_ALIGN_RIGHT,
} engine_font_align_e;

typedef enum
{
  ENGINE_SAMPLER_POINT_CLAMP = 0,
  ENGINE_SAMPLER_POINT_WRAP,
  ENGINE_SAMPLER_LINEAR_CLAMP,
  ENGINE_SAMPLER_LINEAR_WRAP,
  ENGINE_SAMPLE_SIZE,
} engine_sampler_e;

typedef struct engine_image_s
{
  Uint32 id;
} engine_image_t;

void engine_image_setup(engine_context_t *context);

void engine_image_shutdown(void);

engine_image_t engine_image_load(const char *path);

engine_image_t
engine_image_load_mem(unsigned int width, unsigned int height, void *pixels);

void engine_image_destroy(engine_image_t image);

SDL_GPUTexture *engine_image_get_texture(engine_image_t image);

int engine_image_get_width(engine_image_t image);

int engine_image_get_height(engine_image_t image);

#endif // ENGINE_IMAGE_H_
