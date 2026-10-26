#include <boxer/bxr.h>

#include "input.h"
#include "menu.h"
#include "state.h"

static bxr_image_t _title;
static bxr_image_t _start_button;

static bxr_audio_t _music;
static bxr_voice_t _music_voice;

void
menu_setup(void)
{
  _title        = bxr_image_create("assets/title.png");
  _start_button = bxr_image_create("assets/start_button.png");

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
menu_update(double delta_time_ms)
{
  if (input_just_pressed(INPUT_BUTTON_START)) {
    state_set(STATE_LEVEL);
  }
}

void
menu_render(double alpha)
{
  // TODO render images
}

void
menu_teardown(void)
{
  bxr_voice_destroy(_music_voice);
  bxr_audio_destroy(_music);

  bxr_image_destroy(_title);
  bxr_image_destroy(_start_button);
}
