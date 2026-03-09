#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_image.h"
#include "engine_mem.h"
#include "engine_pool.h"

typedef struct _engine_image_s
{
  SDL_GPUTexture *texture;
  int width;
  int height;
} _engine_image_t;

static engine_pool_t *_image_pool;
static _engine_image_t *_images;

static SDL_GPUTransferBuffer *_texture_transfer_buffer;

static Uint32 _initialized        = 0;
static engine_context_t *_context = NULL;

static engine_image_t
_engine_image_load(SDL_Surface *surface)
{
  SDL_assert(surface);

  // Create GPU texture from surface
  Uint8 *texture_transfer_ptr = SDL_MapGPUTransferBuffer(
      _context->gpu_device, _texture_transfer_buffer, true);

  SDL_assert(surface->w * surface->h * 4
             <= ENGINE_RESOURCES_TEXTURE_DIMENSION_MAX
                    * ENGINE_RESOURCES_TEXTURE_DIMENSION_MAX * 4);

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

    engine_set_error(ENGINE_ERROR_IMAGE_LOAD);
    return (engine_image_t){ .id = ENGINE_INVALID_ID };
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

  // Allocate image from pool
  int image_slot = engine_pool_acquire_slot(_image_pool);
  if (image_slot == _ENGINE_INVALID_SLOT) {
    SDL_ReleaseGPUTexture(_context->gpu_device, texture);
    engine_set_error(ENGINE_ERROR_IMAGE_LOAD);
    return (engine_image_t){ .id = ENGINE_INVALID_ID };
  }

  _images[image_slot] = (_engine_image_t){
    .texture = texture,
    .width   = surface->w,
    .height  = surface->h,
  };

  return (engine_image_t){ .id = engine_pool_gen_id(_image_pool, image_slot) };
}

void
engine_image_setup(engine_context_t *context)
{
  SDL_assert(_initialized == 0);
  SDL_assert(context != NULL);

  _initialized = ENGINE_INIT_COOKIE;
  _context     = context;

  _image_pool = engine_pool_make(ENGINE_RESOURCES_IMAGE_MAX);
  ENGINE_CALLOC(_images, ENGINE_RESOURCES_IMAGE_MAX, sizeof(_engine_image_t));

  _texture_transfer_buffer = SDL_CreateGPUTransferBuffer(
      context->gpu_device,
      &(SDL_GPUTransferBufferCreateInfo){
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size  = 4 * ENGINE_RESOURCES_TEXTURE_DIMENSION_MAX
                  * ENGINE_RESOURCES_TEXTURE_DIMENSION_MAX });
  if (!_texture_transfer_buffer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create texture transfer buffer (error: %s)",
                 SDL_GetError());
    engine_set_error(ENGINE_ERROR_IMAGE_SETUP);
    return;
  }
}

void
engine_image_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  engine_pool_destroy(_image_pool);
  ENGINE_FREE(_images);

  SDL_ReleaseGPUTransferBuffer(_context->gpu_device, _texture_transfer_buffer);

  _initialized = 0;
  _context     = NULL;
}

engine_image_t
engine_image_load(const char *path)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(path);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading image: %s", path);

  SDL_IOStream *stream = PHYSFSSDL3_openRead(path);
  if (!stream) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to open image file: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    engine_set_error(ENGINE_ERROR_IMAGE_MAKE);
    return (engine_image_t){ .id = ENGINE_INVALID_ID };
  }

  SDL_Surface *surface = SDL_LoadPNG_IO(stream, true);
  if (!surface) {
    SDL_CloseIO(stream);
    engine_set_error(ENGINE_ERROR_IMAGE_MAKE);
    return (engine_image_t){ .id = ENGINE_INVALID_ID };
  }

  SDL_Log("Loaded filename: %s (%dx%d) format=%s\n",
          path,
          surface->w,
          surface->h,
          SDL_GetPixelFormatName(surface->format));

  // Convert surface to engine pixel format if needed
  if (surface->format != _context->pixel_format) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Converting image pixel format from %s to %s",
                SDL_GetPixelFormatName(surface->format),
                SDL_GetPixelFormatName(_context->pixel_format));

    SDL_Surface *converted_surface
        = SDL_ConvertSurface(surface, _context->pixel_format);
    if (converted_surface == NULL) {
      engine_set_error(ENGINE_ERROR_IMAGE_MAKE);
      return (engine_image_t){ .id = ENGINE_INVALID_ID };
    }

    SDL_DestroySurface(surface);
    surface = converted_surface;
  }

  engine_image_t image = _engine_image_load(surface);

  SDL_DestroySurface(surface);

  return image;
}

engine_image_t
engine_image_load_mem(unsigned int width, unsigned int height, void *pixels)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, _context->pixel_format, pixels, width * 4);
  if (surface == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create surface from memory (error: %s)",
                 SDL_GetError());
    engine_set_error(ENGINE_ERROR_IMAGE_MAKE_MEM);
    return (engine_image_t){ .id = ENGINE_INVALID_ID };
  }

  engine_image_t image = _engine_image_load(surface);

  SDL_DestroySurface(surface);

  return image;
}

void
engine_image_destroy(engine_image_t image)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  if (image.id == ENGINE_INVALID_ID) {
    return;
  }

  int slot_index = engine_pool_id_to_slot(image.id);
  engine_pool_release_slot(_image_pool, slot_index);

  _engine_image_t inner_image = _images[slot_index];

  SDL_ReleaseGPUTexture(_context->gpu_device, inner_image.texture);

  _images[slot_index] = (_engine_image_t){
    .texture = NULL,
    .width   = 0,
    .height  = 0,
  };
}

SDL_GPUTexture *
engine_image_get_texture(engine_image_t image)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(image.id != ENGINE_INVALID_ID);

  int slot_index = engine_pool_id_to_slot(image.id);
  return _images[slot_index].texture;
}

int
engine_image_get_width(engine_image_t image)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(image.id != ENGINE_INVALID_ID);

  int slot_index = engine_pool_id_to_slot(image.id);
  return _images[slot_index].width;
}

int
engine_image_get_height(engine_image_t image)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(image.id != ENGINE_INVALID_ID);

  int slot_index = engine_pool_id_to_slot(image.id);
  return _images[slot_index].height;
}
