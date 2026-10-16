#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_defs.h"
#include "bxr_image.h"
#include "bxr_log.h"
#include "bxr_pixel.h"
#include "bxr_sdl_gp.h"

static Uint32 initialized_ = 0;

void
bxr_image_setup(void)
{
  BXR_ASSERT(initialized_ == 0);
  initialized_ = BXR_INIT_COOKIE;
}

void
bxr_image_teardown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  initialized_ = 0;
}

bxr_image_t
bxr_image_create(const char *path)
{
  BXR_ASSERT(path);
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  BXR_LOG_INFO("Loading image from: %s", path);

  // Check if the file exists
  if (!bxr_asset_exists(path)) {
    BXR_LOG_ERROR("File not found: %s", path);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  // Check if the file has a supported extension
  if (!bxr_asset_has_extension(path, ".png")
      && !bxr_asset_has_extension(path, ".bmp")) {
    BXR_LOG_ERROR("Unsupported image format: %s", path);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  // Open the file using PhysFS
  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    BXR_LOG_ERROR("Failed to open image file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  // Load the image
  SDL_Surface *surface = NULL;
  if (bxr_asset_has_extension(path, ".bmp")) {
    surface = SDL_LoadPNG_IO(stream, true);
  } else if (bxr_asset_has_extension(path, ".png")) {
    surface = SDL_LoadPNG_IO(stream, true);
  }
  if (!surface) {
    BXR_LOG_ERROR(
        "Failed to load image from file: %s (error: %s)", path, SDL_GetError());
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  // Create a GPU image from the surface
  bxr_image_t image = { .id = SDL_GPCreateImage(surface).id };

  SDL_DestroySurface(surface);

  return image;
}

bxr_image_t
bxr_image_create_mem(
    int width, int height, bxr_pixel_format_e format, void *pixels, int pitch)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(width > 0);
  BXR_ASSERT(height > 0);
  BXR_ASSERT(pixels != NULL);

  if (pitch == 0) {
    int bytes_per_pixel = SDL_BYTESPERPIXEL(format);
    if (bytes_per_pixel == 0) {
      BXR_LOG_ERROR("Cannot auto-calculate pitch for format %d", format);
      return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
    }
    pitch = (int)width * bytes_per_pixel;
  }

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, (SDL_PixelFormat)format, pixels, pitch);

  if (!surface) {
    BXR_LOG_ERROR("Failed to create surface from memory (error: %s)",
                  SDL_GetError());
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = { .id = SDL_GPCreateImage(surface).id };

  SDL_DestroySurface(surface);

  return image;
}

int
bxr_image_get_width(bxr_image_t image)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return SDL_GPGetImageWidth((SDL_GPImage){ .id = image.id });
}

int
bxr_image_get_height(bxr_image_t image)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  return SDL_GPGetImageHeight((SDL_GPImage){ .id = image.id });
}

void
bxr_image_destroy(bxr_image_t image)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  SDL_GPDestroyImage((SDL_GPImage){ .id = image.id });
}
