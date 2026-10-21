#include <boxer/bxr.h>

#include "camera.h"

camera_t *
camera_create(bxr_rect_t viewport)
{
  camera_t *camera = NULL;
  BXR_NEW(camera);
  if (!camera) {
    BXR_LOG_ERROR("Failed to create camera");
    return NULL;
  }

  camera->viewport = viewport;

  return camera;
}

void
camera_track(camera_t *camera, bxr_vec2_t target)
{
  BXR_ASSERT(camera != NULL && "camera must not be NULL");

  camera->viewport.x = target.x - (camera->viewport.w * 0.5f);
  camera->viewport.y = target.y - (camera->viewport.h * 0.5f);
}

bxr_rect_t
camera_get_viewport(camera_t *camera)
{
  BXR_ASSERT(camera != NULL && "camera must not be NULL");

  return camera->viewport;
}

void
camera_destroy(camera_t *camera)
{
  if (!camera) {
    return;
  }

  BXR_FREE(camera);
}
