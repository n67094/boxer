#include <boxer/bxr.h>

#include "camera.h"

camera_t *
camera_create(bxr_rect_t viewport, bxr_rect_t bounds)
{
  camera_t *camera = NULL;
  BXR_NEW(camera);
  if (!camera) {
    BXR_LOG_ERROR("Failed to create camera");
    return NULL;
  }

  camera->viewport = viewport;
  camera->bounds   = bounds;

  return camera;
}

static float
camera_clamp_axis(float pos, float view_size, float min, float size)
{
  // Bounds smaller than the view: center them, the camera does not move
  if (size <= view_size) {
    return min + (size - view_size) * 0.5f;
  }

  if (pos < min) {
    return min;
  }
  if (pos > min + size - view_size) {
    return min + size - view_size;
  }

  return pos;
}

void
camera_track(camera_t *camera, bxr_vec2_t target)
{
  BXR_ASSERT(camera != NULL && "camera must not be NULL");

  camera->viewport.x = target.x - (camera->viewport.w * 0.5f);
  camera->viewport.y = target.y - (camera->viewport.h * 0.5f);

  camera->viewport.x = camera_clamp_axis(camera->viewport.x,
                                         camera->viewport.w,
                                         camera->bounds.x,
                                         camera->bounds.w);
  camera->viewport.y = camera_clamp_axis(camera->viewport.y,
                                         camera->viewport.h,
                                         camera->bounds.y,
                                         camera->bounds.h);

  // Snap to whole pixels to avoid seams between tiles
  camera->viewport.x = bxr_floorf(camera->viewport.x);
  camera->viewport.y = bxr_floorf(camera->viewport.y);
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
