#include <boxer/bxr.h>

#include "defs.h"
#include "input.h"
#include "menu.h"

static bxr_image_t _img_background;
static bxr_image_t _img_title;
static bxr_image_t _img_press_start;

static bxr_audio_t _music;
static bxr_voice_t _music_voice;

void
menu_setup(void)
{
  _img_title       = bxr_image_create(ASSET_TITLE);
  _img_background  = bxr_image_create(ASSET_TITLE_BACKGROUND);
  _img_press_start = bxr_image_create(ASSET_TITLE_PRESS_START);

  _music = bxr_audio_create("assets/music.ogg");

  bxr_voice_desc_t music_desc = {
    .audio      = _music,
    .volume     = 1.0f,
    .auto_play  = true,
    .keep_alive = true,
  };

  _music_voice = bxr_voice_create(&music_desc);
}

void
menu_update(double delta_ms)
{
  if (input_just_pressed(INPUT_BUTTON_START)) {
    scene_go_to(SCENE_LEVEL);
  }
}

void
menu_render(double alpha_ms)
{
  // TODO render images
}

void
menu_teardown(void)
{
  bxr_voice_destroy(_music_voice);
  bxr_audio_destroy(_music);

  bxr_image_destroy(_img_press_start);
  bxr_image_destroy(_img_title);
  bxr_image_destroy(_img_background);
}
