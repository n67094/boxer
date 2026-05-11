#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_image.h"

static Uint32 initialized_     = 0;
static bxr_context_t *context_ = NULL;

void
bxr_image_setup(bxr_context_t *context)
{
  SDL_assert(initialized_ == 0);
  SDL_assert(context != NULL);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;
}

void
bxr_image_shutdown(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  initialized_ = 0;
  context_     = NULL;
}

bxr_image_t
bxr_image_create(const char *path)
{
  SDL_assert(path);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading image from: %s", path);

  if (!PHYSFS_exists(path)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File not found: %s", path);
    bxr_error_set(BXR_ERROR_FILE_NOT_FOUND);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to open image file: %s (error: %s)",
                 path,
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    bxr_error_set(BXR_ERROR_IMAGE_READ);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  SDL_Surface *surface = SDL_LoadPNG_IO(stream, true);
  if (!surface) {
    SDL_CloseIO(stream);
    bxr_error_set(BXR_ERROR_IMAGE_CREATE);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = SDL_GPCreateImage(surface);

  SDL_DestroySurface(surface);

  return image;
}

bxr_image_t
bxr_image_create_mem(unsigned int width, unsigned int height, void *pixels)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, context_->pixel_format, pixels, width * 4);

  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create surface from memory (error: %s)",
                 SDL_GetError());
    bxr_error_set(BXR_ERROR_IMAGE_CREATE_MEM);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = SDL_GPCreateImage(surface);

  SDL_DestroySurface(surface);

  return image;
}

int
bxr_image_get_width(bxr_image_t image)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  return SDL_GPGetImageWidth((SDL_GPImage)image);
}

int
bxr_image_get_height(bxr_image_t image)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  return SDL_GPGetImageHeight((SDL_GPImage)image);
}

void
bxr_image_destroy(bxr_image_t image)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  SDL_GPDestroyImage((SDL_GPImage)image);
}
