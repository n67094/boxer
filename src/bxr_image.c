#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include <SDL_gp.h>

#include "bxr_assert.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_image.h"
#include "bxr_log.h"

static Uint32 initialized_     = 0;
static bxr_context_t *context_ = NULL;

void
bxr_image_setup(bxr_context_t *context)
{
  BXR_ASSERT(initialized_ == 0);
  BXR_ASSERT(context != NULL);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;
}

void
bxr_image_shutdown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  initialized_ = 0;
  context_     = NULL;
}

bxr_image_t
bxr_image_create(const char *path)
{
  BXR_ASSERT(path);

  BXR_LOG_INFO("Loading image from: %s", path);

  if (!PHYSFS_exists(path)) {
    BXR_LOG_ERROR("File not found: %s", path);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    BXR_LOG_ERROR("Failed to open image file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  SDL_Surface *surface = SDL_LoadPNG_IO(stream, true);
  if (!surface) {
    BXR_LOG_ERROR(
        "Failed to load image from file: %s (error: %s)", path, SDL_GetError());
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = { .id = SDL_GPCreateImage(surface).id };

  SDL_DestroySurface(surface);

  return image;
}

bxr_image_t
bxr_image_create_mem(unsigned int width, unsigned int height, void *pixels)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, context_->pixel_format, pixels, width * 4);

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
