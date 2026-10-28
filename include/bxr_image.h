/**
 * File `bxr_image.h`.
 *
 * # Boxer's Image Management.
 *
 * Images are handles (`bxr_image_t`) created from asset files mounted with the
 * `bxr_asset.h` API or from raw pixels in memory, and released with
 * `bxr_image_destroy()`. A handle whose `id` is `BXR_IMAGE_INVALID_ID` is
 * invalid.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_IMAGE_H_
#define BXR_IMAGE_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"
#include "bxr_pixel.h"

#define BXR_IMAGE_INVALID_ID 0

typedef struct bxr_image_s
{
  Uint32 id;
} bxr_image_t;

bool bxr_image_setup(void);
void bxr_image_teardown(void);

/**
 * ## Public API
 */

/**
 * Load a PNG image from a mounted asset file.
 *
 * The pixels are converted to the swapchain format and uploaded to the GPU.
 *
 * `path` path of the file, relative to the mounted directories. Must not be
 * `NULL` and must end with `.png` or `.bmp`.
 *
 * `return` the image, or an image with id `BXR_IMAGE_INVALID_ID` on failure.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller must destroy the result with `bxr_image_destroy()`.
 */
BXR_API bxr_image_t bxr_image_create(const char *path);

/**
 * Create an image from raw pixels in memory.
 *
 * The pixels are copied, converted to the swapchain format if needed, and
 * uploaded to the GPU.
 *
 * `width` width in pixels.
 * `height` height in pixels.
 * `format` pixel format of `pixels` (e.g. `BXR_PIXEL_FORMAT_RGBA32`).
 * `pixels` pixel data, at least `pitch * height` bytes. Remains owned by the
 * caller and may be freed once the call returns.
 * `pitch` bytes per row, or 0 to compute it as `width` times the format's
 * bytes per pixel.
 *
 * `return` the image, or an image with id `BXR_IMAGE_INVALID_ID` on failure.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller must destroy the result with `bxr_image_destroy()`.
 */
BXR_API bxr_image_t bxr_image_create_mem(
    int width, int height, bxr_pixel_format_e format, void *pixels, int pitch);

/**
 * Get the width of an image.
 *
 * `image` image to query.
 *
 * `return` the width in pixels, or 0 if the image id is `BXR_IMAGE_INVALID_ID`.
 */
BXR_API int bxr_image_get_width(bxr_image_t image);

/**
 * Get the height of an image.
 *
 * `image` image to query.
 *
 * `return` the height in pixels, or 0 if the image id is
 * `BXR_IMAGE_INVALID_ID`.
 */
BXR_API int bxr_image_get_height(bxr_image_t image);

/**
 * Check if an image handle is valid.
 *
 * `image` image to check.
 *
 * `return` true if the image id is not `BXR_IMAGE_INVALID_ID`, false otherwise.
 */
BXR_API bool bxr_image_is_valid(bxr_image_t image);

/**
 * Destroy an image and release its GPU texture.
 *
 * `image` image to destroy.
 */
BXR_API void bxr_image_destroy(bxr_image_t image);

#endif // BXR_IMAGE_H_
