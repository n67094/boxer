#include <boxer/bxr.h>

#include "defs.h"
#include "input.h"
#include "title.h"

#define PRESS_START_BLINK 500

static bxr_image_t _img_background;
static bxr_image_t _img_title;
static bxr_image_t _img_press_start;

static Uint64 _blink_acc = 0;
static bool _blink       = true;

void
title_setup(void)
{
  _img_title       = bxr_image_create(ASSET_TITLE);
  _img_background  = bxr_image_create(ASSET_TITLE_BACKGROUND);
  _img_press_start = bxr_image_create(ASSET_TITLE_PRESS_START);
}

void
title_update(double delta_ms)
{
  if (input_just_pressed(INPUT_BUTTON_START)) {
    scene_go_to(SCENE_LEVEL);
  }

  // Handle blinking for splash screen message
  _blink_acc += (Uint64)delta_ms;
  if (_blink_acc >= PRESS_START_BLINK) {
    _blink_acc = 0;
    _blink     = !_blink;
  }
}

void
title_render(double alpha_ms)
{
  bxr_painter_set_blendmode(BXR_BLENDMODE_BLEND);
  bxr_painter_set_color(BXR_COLOR_WHITE);

  // Draw the background
  bxr_painter_set_image(0, _img_background);
  bxr_painter_draw_textured_rect(
      0,
      (bxr_textured_rect_t){
          .dst = (bxr_rect_t){ .x = 0,
                               .y = 0,
                               .w = WINDOW_VIRTUAL_WIDTH,
                               .h = WINDOW_VIRTUAL_HEIGHT },
          .src = (bxr_rect_t){ .x = 0,
                               .y = 0,
                               .w = bxr_image_get_width(_img_background),
                               .h = bxr_image_get_height(_img_background) } });

  // Draw the title
  bxr_painter_set_image(0, _img_title);
  bxr_painter_draw_textured_rect(
      0,
      (bxr_textured_rect_t){
          .dst = (bxr_rect_t){ .x = 8,
                               .y = 24,
                               .w = bxr_image_get_width(_img_title),
                               .h = bxr_image_get_height(_img_title) },
          .src = (bxr_rect_t){ .x = 0,
                               .y = 0,
                               .w = bxr_image_get_width(_img_title),
                               .h = bxr_image_get_height(_img_title) } });

  // Draw the "Press Start" prompt every other half second
  if (_blink) {
    bxr_painter_set_image(0, _img_press_start);
    bxr_painter_draw_textured_rect(
        0,
        (bxr_textured_rect_t){
            .dst = (bxr_rect_t){ .x = 24,
                                 .y = 96,
                                 .w = bxr_image_get_width(_img_press_start),
                                 .h = bxr_image_get_height(_img_press_start) },
            .src
            = (bxr_rect_t){ .x = 0,
                            .y = 0,
                            .w = bxr_image_get_width(_img_press_start),
                            .h = bxr_image_get_height(_img_press_start) } });
  }

  bxr_painter_reset_image(0);
  bxr_painter_set_blendmode(BXR_BLENDMODE_NONE);
}

void
title_teardown(void)
{

  bxr_image_destroy(_img_press_start);
  bxr_image_destroy(_img_title);
  bxr_image_destroy(_img_background);
}
