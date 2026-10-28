/**
 * @file animation.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <boxer/bxr.h>

typedef struct clip_s
{
  int from;
  int to;
  double frame_ms; // Duration of a frame in milliseconds
} clip_t;

typedef struct animation_s
{
  bxr_image_t spritesheet;
  Uint32 frame_width;
  Uint32 frame_height;
  const clip_t *clips;
  Uint32 clip_count;
  int current_clip;
  int current_frame;
  double elapsed_ms; // Elapsed time since the last frame change
  bool flip_x;
} animation_t;

typedef struct animation_desc_s
{
  const char *spritesheet_path;
  Uint32 frame_width;
  Uint32 frame_height;
  const clip_t *clips;
  Uint32 clip_count;
} animation_desc_t;

/**
 * Creates a new animation with the given clips.
 *
 * `desc` is a pointer to the animation description that defines the animation's
 * properties.
 *
 * `return` a pointer to the newly created animation, or NULL otherwise.
 */
animation_t *animation_create(animation_desc_t *desc);

/**
 * Sets the current animation clip to the specified index.
 *
 * `animation` is a pointer to the animation to modify.
 *
 * `index` is the index of the clip to set as the current clip.
 */
void animation_set_clip(animation_t *animation, Uint32 index);

/**
 * Updates the animation based on the elapsed time since the last update.
 *
 * `animation` is a pointer to the animation to update.
 *
 * `delta_ms` is the time elapsed since the last update in milliseconds.
 */
void animation_update(animation_t *animation, Uint64 delta_ms);

/**
 * Renders the current frame of the animation at the specified position.
 *
 * `animation` is a pointer to the animation to update.
 *
 * `position` is the position in world space where the animation should be
 * rendered.
 */
void animation_render(animation_t *animation, bxr_vec2_t position);

/**
 * Destroys the given animation and frees its resources.
 *
 * `animation` is a pointer to the animation to destroy.
 */
void animation_destroy(animation_t *animation);

#endif // ANIMATION_H_
