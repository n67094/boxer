/**
 * @file bxr_image.h
 *
 * @brief Image loading and management.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_IMAGE_H_
#define BXR_IMAGE_H_

#include <SDL3/SDL_gpu.h>

#include "bxr_context.h"

typedef enum
{
  BXR_SAMPLER_POINT_CLAMP = 0,
  BXR_SAMPLER_POINT_WRAP,
  BXR_SAMPLER_LINEAR_CLAMP,
  BXR_SAMPLER_LINEAR_WRAP,
  BXR_SAMPLER_SIZE,
} bxr_sampler_e;

typedef struct bxr_image_s
{
  Uint32 id;
} bxr_image_t;

void bxr_image_setup(bxr_context_t *context);

void bxr_image_shutdown(void);

bxr_image_t bxr_image_load(const char *path);

bxr_image_t
bxr_image_load_mem(unsigned int width, unsigned int height, void *pixels);

void bxr_image_destroy(bxr_image_t image);

SDL_GPUTexture *bxr_image_get_texture(bxr_image_t image);

int bxr_image_get_width(bxr_image_t image);

int bxr_image_get_height(bxr_image_t image);

#endif // BXR_IMAGE_H_
