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
} camera_t;

/**
 * Creates a new camera with the given viewport.
 *
 * `viewport` is the rectangle that defines the camera's view in world space.
 *
 * Returns a pointer to the newly created camera, or NULL if an error occurred.
 */
camera_t *camera_create(bxr_rect_t viewport);

/**
 * Updates the camera's position to track the given target.
 *
 * `camera` is a pointer to the camera to update.
 *
 * `target` is the position in world space that the camera should track.
 *
 * The camera will center its viewport on the target position.
 */
void camera_track(camera_t *camera, bxr_vec2_t target);

/**
 * Gets the camera's current viewport.
 *
 * `camera` is a pointer to the camera to query.
 *
 * Returns  the cemera's viewport rectangle (in window virtual dimensions).
 */
bxr_rect_t camera_get_viewport(camera_t *camera);

/**
 * Destroys the given camera and frees its resources.
 */
void camera_destroy(camera_t *camera);

#endif // CAMERA_H_
