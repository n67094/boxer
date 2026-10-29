/**
 * File `camera.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <boxer/bxr.h>

typedef struct camera_s
{
  bxr_rect_t viewport;
  bxr_rect_t bounds;
} camera_t;

/**
 * Creates a new camera.
 *
 * `viewport` is the rectangle that defines the camera's view in world space.
 *
 * `bounds` is the rectangle that defines the camera's bounds in world space.
 *
 * `return` a pointer to the newly created camera, or NULL if an error occurred.
 */
camera_t *camera_create(bxr_rect_t viewport, bxr_rect_t bounds);

/**
 * Updates the camera's position to track the given target.
 *
 * `camera` is a pointer to the camera to update.
 *
 * `target` is the position in world space that the camera should track.
 */
void camera_track(camera_t *camera, bxr_vec2_t target);

/**
 * Gets the camera's current viewport.
 *
 * `camera` is a pointer to the camera to query.
 *
 *  `return` a bxr_rect_t that describes the camera's current viewport in world
 * space.
 */
bxr_rect_t camera_get_viewport(camera_t *camera);

void camera_destroy(camera_t *camera);

#endif // CAMERA_H_
