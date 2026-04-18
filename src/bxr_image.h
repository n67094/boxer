/**
 * file `bxr_image.h`
 *
 * copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Image management and utilities.
 */

#ifndef BXR_IMAGE_H_
#define BXR_IMAGE_H_

#include <SDL_gp.h>

#include "bxr_context.h"

#define BXR_IMAGE_INVALID_ID SDL_GP_INVALID_ID

typedef enum
{
  BXR_SAMPLER_POINT_CLAMP  = SDL_GP_SAMPLER_POINT_CLAMP,
  BXR_SAMPLER_POINT_WRAP   = SDL_GP_SAMPLER_POINT_WRAP,
  BXR_SAMPLER_LINEAR_CLAMP = SDL_GP_SAMPLER_LINEAR_CLAMP,
  BXR_SAMPLER_LINEAR_WRAP  = SDL_GP_SAMPLER_LINEAR_WRAP,
  BXR_GP_SAMPLER_SIZE      = SDL_GP_SAMPLER_SIZE,
} bxr_sampler_e;

typedef SDL_GPImage bxr_image_t;

/**
 * ## Internal API
 */

/**
 * Initialize the image subsystem. Must be called before using any other image
 * functions.
 *
 * `context` is the bxr context to use for image operations.
 */
void bxr_image_setup(bxr_context_t *context);

/**
 * Shutdown the image subsystem.
 */
void bxr_image_shutdown(void);

/**
 * ## Public API
 */

/**
 * Load an image from a file.
 *
 * `path` is the path to the image file to load, relative to the data directory.
 *
 * `return` the loaded image, or an invalid image if the image could not be
 * loaded.
 */
bxr_image_t bxr_image_make(const char *path);

/**
 * Create an image from memory.
 *
 * `width` is the width of the image in pixels.
 *
 * `height` is the height of the image in pixels.
 *
 * `pixels` is a pointer to the pixel data in memory.
 *
 * `return` the created image, or an invalid image if the image could not be
 * created.
 */
bxr_image_t
bxr_image_make_mem(unsigned int width, unsigned int height, void *pixels);

/**
 * Get the width of an image in pixels.
 *
 * `image` is the image to get the width of.
 *
 * `return` the width of the image in pixels, or 0 if the image is invalid.
 */
int bxr_image_get_width(bxr_image_t image);

/**
 * Get the height of an image in pixels.
 *
 * `image` is the image to get the height of.
 *
 * `return` the height of the image in pixels, or 0 if the image is invalid.
 */
int bxr_image_get_height(bxr_image_t image);

/**
 * Destroy an image and free its resources.
 *
 * `image` is the image to destroy.
 */
void bxr_image_destroy(bxr_image_t image);

#endif // BXR_IMAGE_H_
