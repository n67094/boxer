/**
 * File `bxr_sampler.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Sampler Management for Texture Sampling in Shaders
 */

#ifndef BXR_SAMPLER_H_
#define BXR_SAMPLER_H_

#include <SDL3/SDL.h>

typedef enum
{
  BXR_SAMPLER_POINT_CLAMP = 0,
  BXR_SAMPLER_POINT_WRAP,
  BXR_SAMPLER_LINEAR_CLAMP,
  BXR_SAMPLER_LINEAR_WRAP,
  BXR_GP_SAMPLER_SIZE,
} bxr_sampler_e;

typedef struct bxr_sampler_s
{
  SDL_GPUSampler *sampler;
} bxr_sampler_t;

/**
 * ## Internal API
 */

/**
 * Initializes the sampler system. Must be called before using any other sampler
 * functions.
 */
void bxr_sampler_setup(void);

/**
 * Tears down the sampler system.
 */
void bxr_sampler_teardown(void);

/**
 * ## Public API
 */

/**
 * Create a sampler with the given sampler type.
 *
 * `sampler_type` is the type of sampler to create.
 *
 * `return` the created sampler, or an NULL sampler if the sampler could not
 * be created.
 */
bxr_sampler_t bxr_sampler_create(bxr_sampler_e sampler_type);

/**
 * Destroy a sampler.
 *
 * `sampler` is the sampler to destroy.
 */
void bxr_sampler_destroy(bxr_sampler_t *sampler);

#endif // BXR_SAMPLER_H_
