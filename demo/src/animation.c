#include "animation.h"

animation_t *
animation_create(animation_desc_t *desc)
{
  BXR_ASSERT(desc != NULL && "desc must not be NULL");
  BXR_ASSERT(desc->spritesheet_path != NULL
             && "spritesheet_path must not be NULL");
  BXR_ASSERT(desc->clips != NULL && "clips must not be NULL");
  BXR_ASSERT(desc->clip_count > 0 && "clip_count must be greater than 0");

  animation_t *animation = NULL;
  BXR_NEW0(animation);
  if (!animation) {
    BXR_LOG_ERROR("Failed to create animation");
    return NULL;
  }

  animation->spritesheet   = bxr_image_create(desc->spritesheet_path);
  animation->frame_width   = desc->frame_width;
  animation->frame_height  = desc->frame_height;
  animation->clips         = BXR_ALLOC(sizeof(clip_t) * desc->clip_count);
  animation->clip_count    = desc->clip_count;
  animation->current_clip  = 0;
  animation->current_frame = 0;
  animation->elapsed_ms    = 0.0f;
  animation->flip_x        = false;

  BXR_MEMCPY(animation->clips, desc->clips, sizeof(clip_t) * desc->clip_count);

  return animation;
}

void
animation_set_clip(animation_t *animation, Uint32 index)
{
  BXR_ASSERT(animation != NULL && "animation must not be NULL");
  BXR_ASSERT(index < animation->clip_count && "index out of bounds");

  animation->current_clip  = index;
  animation->current_frame = 0;
  animation->elapsed_ms    = 0.0f;
}

void
animation_update(animation_t *animation, Uint64 delta_ms)
{
  BXR_ASSERT(animation != NULL && "animation must not be NULL");

  clip_t *clip = &animation->clips[animation->current_clip];

  animation->elapsed_ms += delta_ms;

  if (animation->elapsed_ms >= clip->frame_ms) {
    animation->elapsed_ms -= clip->frame_ms;
    animation->current_frame++;

    if (animation->current_frame > (clip->to - clip->from)) {
      animation->current_frame = 0; // Loop back to the first frame of the clip
    }
  }
}

void
animation_render(animation_t *animation, bxr_vec2_t position)
{
  BXR_ASSERT(animation != NULL && "animation must not be NULL");

  clip_t *clip = &animation->clips[animation->current_clip];

  Uint32 frame_index = clip->from + animation->current_frame;

  int tileset_w = bxr_image_get_width(animation->spritesheet);

  int src_x = (frame_index % (tileset_w / animation->frame_width))
              * animation->frame_width;
  int src_y = (frame_index / (tileset_w / animation->frame_width))
              * animation->frame_height;

  bxr_rect_t src_rect = {
    .x = src_x,
    .y = src_y,
    .w = animation->frame_width,
    .h = animation->frame_height,
  };

  if (animation->flip_x) {
    src_rect.x += src_rect.w;
    src_rect.w = -src_rect.w;
  }

  bxr_painter_set_blendmode(BXR_BLENDMODE_BLEND);
  bxr_painter_set_color(BXR_COLOR_WHITE);
  bxr_painter_set_image(0, animation->spritesheet);

  bxr_painter_draw_textured_rect(0, (bxr_textured_rect_t){
    .dst = (bxr_rect_t){
      .x = position.x,
      .y = position.y,
      .w = animation->frame_width,
      .h = animation->frame_height,
    },
    .src = src_rect,
  });

  bxr_painter_reset_image(0);
  bxr_painter_set_blendmode(BXR_BLENDMODE_NONE);
}

void
animation_destroy(animation_t *animation)
{
  if (!animation) {
    return;
  }

  bxr_image_destroy(animation->spritesheet);
  BXR_FREE(animation->clips);
  BXR_FREE(animation);
}
