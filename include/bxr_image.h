/**
 * File `bxr_image.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Image Management and Utilities
 *
 * Image are loaded from mounted directories using `bxr_asset.h` API.
 */

#ifndef BXR_IMAGE_H_
#define BXR_IMAGE_H_

#include "bxr_context.h"
#include "bxr_defs.h"

#define BXR_IMAGE_INVALID_ID 0

typedef struct bxr_image_s
{
  Uint32 id;
} bxr_image_t;

/**
 * ## Internal API
 */

/**
 * Initialize the image subsystem. Must be called before using any other image
 * functions.
 *
 * `context` is the bxr context to use for image operations.
 */
BXR_API void bxr_image_setup(bxr_context_t *context);

/**
 * Shutdown the image subsystem.
 */
BXR_API void bxr_image_shutdown(void);

/**
 * ## Public API
 */

/**
 * Load an image from a file.
 *
 * `path` is the path to the image file to load, relative mounted directories.
 *
 * `return` the loaded image, or an invalid image if the image could not be
 * loaded. Use `bxr_error_get` to get more information about the error.
 *
 * The caller is responsible for destroying the returned image using
 * `bxr_image_destroy` when it is no longer needed.
 */
BXR_API bxr_image_t bxr_image_create(const char *path);

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
 * created. Use `bxr_error_get` to get more information about the error.
 *
 * The caller is responsible for destroying the returned image using
 * `bxr_image_destroy` when it is no longer needed.
 */
BXR_API bxr_image_t bxr_image_create_mem(unsigned int width,
                                         unsigned int height,
                                         void *pixels);

/**
 * Get the width of an image in pixels.
 *
 * `image` is the image to get the width of.
 *
 * `return` the width of the image in pixels, or 0 if the image is invalid.
 */
BXR_API int bxr_image_get_width(bxr_image_t image);

/**
 * Get the height of an image in pixels.
 *
 * `image` is the image to get the height of.
 *
 * `return` the height of the image in pixels, or 0 if the image is invalid.
 */
BXR_API int bxr_image_get_height(bxr_image_t image);

/**
 * Destroy an image and free its resources.
 *
 * `image` is the image to destroy.
 */
BXR_API void bxr_image_destroy(bxr_image_t image);

#endif // BXR_IMAGE_H_
