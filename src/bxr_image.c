#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_image.h"

static Uint32 _initialized     = 0;
static bxr_context_t *_context = NULL;

void
bxr_image_setup(bxr_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = BXR_INIT_COOKIE;
  _context     = context;
}

void
bxr_image_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  _initialized = 0;
  _context     = NULL;
}

bxr_image_t
bxr_image_make(const char *path)
{
  SDL_assert(path);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading image from: %s", path);

  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to open image file: %s (error: %s)",
                path,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    bxr_error_set(BXR_ERROR_IMAGE_FILE);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  SDL_Surface *surface = SDL_LoadPNG_IO(stream, true);
  if (!surface) {
    SDL_CloseIO(stream);
    bxr_error_set(BXR_ERROR_IMAGE_MAKE);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = SDL_GPCreateImage(surface);

  SDL_DestroySurface(surface);

  return image;
}

bxr_image_t
bxr_image_make_mem(unsigned int width, unsigned int height, void *pixels)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, _context->pixel_format, pixels, width * 4);

  if (surface == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create surface from memory (error: %s)",
                 SDL_GetError());
    bxr_error_set(BXR_ERROR_IMAGE_MAKE_MEM);
    return (bxr_image_t){ .id = BXR_IMAGE_INVALID_ID };
  }

  bxr_image_t image = SDL_GPCreateImage(surface);

  SDL_DestroySurface(surface);

  return image;
}

int
bxr_image_get_width(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  return SDL_GPGetImageWidth((SDL_GPImage)image);
}

int
bxr_image_get_height(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  return SDL_GPGetImageHeight((SDL_GPImage)image);
}

void
bxr_image_destroy(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_GPDestroyImage((SDL_GPImage)image);
}
