#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_image.h"
#include "bxr_mem.h"
#include "bxr_resource.h"

typedef struct _bxr_image_s
{
  SDL_GPUTexture *texture;
  int width;
  int height;
} _bxr_image_t;

static bxr_resource_t *_image_resource;
static _bxr_image_t *_images;

static SDL_GPUTransferBuffer *_texture_transfer_buffer;

static Uint32 _initialized     = 0;
static bxr_context_t *_context = NULL;

static bxr_image_t
_bxr_image_load(SDL_Surface *surface)
{
  SDL_assert(surface);

  // Create GPU texture from surface
  Uint8 *texture_transfer_ptr = SDL_MapGPUTransferBuffer(
      _context->gpu_device, _texture_transfer_buffer, true);

  SDL_assert(surface->w * surface->h * 4
             <= BXR_RESOURCES_TEXTURE_DIMENSION_MAX
                    * BXR_RESOURCES_TEXTURE_DIMENSION_MAX * 4);

  SDL_memcpy(
      texture_transfer_ptr, surface->pixels, surface->w * surface->h * 4);

  SDL_UnmapGPUTransferBuffer(_context->gpu_device, _texture_transfer_buffer);

  // Create the GPU resources
  SDL_GPUTexture *texture = SDL_CreateGPUTexture(
      _context->gpu_device,
      &(SDL_GPUTextureCreateInfo){ .type   = SDL_GPU_TEXTURETYPE_2D,
                                   .format = _context->texture_format,
                                   .width  = surface->w,
                                   .height = surface->h,
                                   .layer_count_or_depth = 1,
                                   .num_levels           = 1,
                                   .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER });
  if (texture == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create GPU texture for image (error: %s)",
                 SDL_GetError());

    bxr_set_error(BXR_ERROR_IMAGE_LOAD);
    return (bxr_image_t){ .id = BXR_INVALID_ID };
  }

  SDL_assert(_context->cmd_buffer);

  SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(_context->cmd_buffer);

  SDL_UploadToGPUTexture(
      copy_pass,
      &(SDL_GPUTextureTransferInfo){
          .transfer_buffer = _texture_transfer_buffer,
          .offset          = 0,
      },
      &(SDL_GPUTextureRegion){
          .texture = texture, .w = surface->w, .h = surface->h, .d = 1 },
      false);

  SDL_EndGPUCopyPass(copy_pass);

  // Allocate image from resource
  int image_slot = bxr_resource_acquire_slot(_image_resource);
  if (image_slot == _BXR_RESOURCE_INVALID_SLOT) {
    SDL_ReleaseGPUTexture(_context->gpu_device, texture);
    bxr_set_error(BXR_ERROR_IMAGE_LOAD);
    return (bxr_image_t){ .id = BXR_INVALID_ID };
  }

  _images[image_slot] = (_bxr_image_t){
    .texture = texture,
    .width   = surface->w,
    .height  = surface->h,
  };

  return (bxr_image_t){ .id
                        = bxr_resource_gen_id(_image_resource, image_slot) };
}

void
bxr_image_setup(bxr_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = BXR_INIT_COOKIE;
  _context     = context;

  _image_resource = bxr_resource_make(BXR_RESOURCES_IMAGE_MAX);
  BXR_CALLOC(_images, BXR_RESOURCES_IMAGE_MAX, sizeof(_bxr_image_t));

  _texture_transfer_buffer = SDL_CreateGPUTransferBuffer(
      context->gpu_device,
      &(SDL_GPUTransferBufferCreateInfo){
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size  = 4 * BXR_RESOURCES_TEXTURE_DIMENSION_MAX
                   * BXR_RESOURCES_TEXTURE_DIMENSION_MAX });
  if (!_texture_transfer_buffer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create texture transfer buffer (error: %s)",
                 SDL_GetError());
    bxr_set_error(BXR_ERROR_IMAGE_SETUP);
    return;
  }
}

void
bxr_image_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  bxr_resource_destroy(_image_resource);
  BXR_FREE(_images);

  SDL_ReleaseGPUTransferBuffer(_context->gpu_device, _texture_transfer_buffer);

  _initialized = 0;
  _context     = NULL;
}

bxr_image_t
bxr_image_load(const char *path)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(path);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading image: %s", path);

  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to open image file: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    bxr_set_error(BXR_ERROR_IMAGE_MAKE);
    return (bxr_image_t){ .id = BXR_INVALID_ID };
  }

  SDL_Surface *surface = SDL_LoadPNG_IO(stream, true);
  if (!surface) {
    SDL_CloseIO(stream);
    bxr_set_error(BXR_ERROR_IMAGE_MAKE);
    return (bxr_image_t){ .id = BXR_INVALID_ID };
  }

  SDL_Log("Loaded filename: %s (%dx%d) format=%s\n",
          path,
          surface->w,
          surface->h,
          SDL_GetPixelFormatName(surface->format));

  // Convert surface to bxr pixel format if needed
  if (surface->format != _context->pixel_format) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Converting image pixel format from %s to %s",
                SDL_GetPixelFormatName(surface->format),
                SDL_GetPixelFormatName(_context->pixel_format));

    SDL_Surface *converted_surface
        = SDL_ConvertSurface(surface, _context->pixel_format);
    if (converted_surface == NULL) {
      bxr_set_error(BXR_ERROR_IMAGE_MAKE);
      return (bxr_image_t){ .id = BXR_INVALID_ID };
    }

    SDL_DestroySurface(surface);
    surface = converted_surface;
  }

  bxr_image_t image = _bxr_image_load(surface);

  SDL_DestroySurface(surface);

  return image;
}

bxr_image_t
bxr_image_load_mem(unsigned int width, unsigned int height, void *pixels)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, _context->pixel_format, pixels, width * 4);
  if (surface == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create surface from memory (error: %s)",
                 SDL_GetError());
    bxr_set_error(BXR_ERROR_IMAGE_MAKE_MEM);
    return (bxr_image_t){ .id = BXR_INVALID_ID };
  }

  bxr_image_t image = _bxr_image_load(surface);

  SDL_DestroySurface(surface);

  return image;
}

void
bxr_image_destroy(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  if (image.id == BXR_INVALID_ID) {
    return;
  }

  int slot_index = bxr_resource_id_to_slot(image.id);
  bxr_resource_release_slot(_image_resource, slot_index);

  _bxr_image_t inner_image = _images[slot_index];

  SDL_ReleaseGPUTexture(_context->gpu_device, inner_image.texture);

  _images[slot_index] = (_bxr_image_t){
    .texture = NULL,
    .width   = 0,
    .height  = 0,
  };
}

SDL_GPUTexture *
bxr_image_get_texture(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  // TODO return NULL instead of asserting
  SDL_assert(image.id != BXR_INVALID_ID);

  int slot_index = bxr_resource_id_to_slot(image.id);
  return _images[slot_index].texture;
}

int
bxr_image_get_width(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  // TODO return NULL instead of asserting
  SDL_assert(image.id != BXR_INVALID_ID);

  int slot_index = bxr_resource_id_to_slot(image.id);
  return _images[slot_index].width;
}

int
bxr_image_get_height(bxr_image_t image)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  // TODO return NULL instead of asserting
  SDL_assert(image.id != BXR_INVALID_ID);

  int slot_index = bxr_resource_id_to_slot(image.id);
  return _images[slot_index].height;
}
