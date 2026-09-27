#include <boxer/bxr.h>

#define ASSET_MOUNT "assets.zip"
#define ASSET_PATH "assets/"

static bxr_image_t _image;
static bxr_audio_t _audio;

static bxr_voice_t _music;

static bxr_game_config_t _config = { 0 };

bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "boxer_bxr";
  _config.title = "Boxer Engine";

  _config.width  = 1280;
  _config.height = 720;

  _config.virtual_width  = 1280 * 0.5;
  _config.virtual_height = 720 * 0.5;

  _config.fullscreen = false;
  _config.resizable  = false;

  return &_config;
}

void
bxr_game_setup(void)
{
  bxr_asset_mount(ASSET_MOUNT);

  _image = bxr_image_create(ASSET_PATH "images/logo.png");
  _audio = bxr_audio_create(ASSET_PATH "audio/music.wav");

  bxr_voice_desc_t music_desc = {
    .audio     = _audio,
    .volume    = 1.0f,
    .auto_play = true,
    .play_once = false,
  };

  _music = bxr_voice_create(&music_desc);
}

void
bxr_game_update(Uint64 delta_time_ms)
{
  BXR_LOG_INFO("Use Space to pause/resume the music voice");
  BXR_LOG_INFO("Use Left to decrease the volume of the music voice");
  BXR_LOG_INFO("Use Right to increase the volume of the music voice");
  BXR_LOG_INFO("--- Track Information -----------------------------");
  BXR_LOG_INFO("Length: %u s", bxr_audio_get_length(_audio) / 1000);
  BXR_LOG_INFO("Position: %u ms", bxr_voice_get_position(_music));
  BXR_LOG_INFO("Volume: %f", bxr_voice_get_volume(_music));
  BXR_LOG_INFO("Status : %s",
               bxr_voice_is_paused(_music) ? "PAUSED" : "PLAYING");

  if (bxr_key_just_pressed(BXR_KEY_SPACE)
      || bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_SOUTH)) {
    if (bxr_voice_is_paused(_music)) {
      bxr_voice_play(_music);
    } else {
      bxr_voice_pause(_music);
    }
  }

  if (bxr_key_just_pressed(BXR_KEY_LEFT)
      || bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_LEFT)) {
    float volume = bxr_voice_get_volume(_music);
    volume -= 0.1f;
    if (volume < 0.0f) {
      volume = 0.0f;
    }
    bxr_voice_set_volume(_music, volume);
  }

  if (bxr_key_just_pressed(BXR_KEY_RIGHT)
      || bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_RIGHT)) {
    float volume = bxr_voice_get_volume(_music);
    volume += 0.1f;
    if (volume > 1.0f) {
      volume = 1.0f;
    }
    bxr_voice_set_volume(_music, volume);
  }
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
  bxr_vec2_t window_size = bxr_context_get_virtual_window_dimensions();

  bxr_painter_set_color(bxr_color_create(0, 0, 0, 255));
  bxr_painter_clear();

  for (int i = 0; i < 1024; ++i) {
    float width  = window_size.x;
    float height = window_size.y;

    int img_width  = bxr_image_get_width(_image);
    int img_height = bxr_image_get_height(_image);

    bxr_painter_set_color(bxr_color_create(255, 255, 255, 255));

    int x = SDL_rand(width);
    int y = SDL_rand(height);

    bxr_rect_t src_rect
        = bxr_rect_create(0, 0, (float)img_width, (float)img_height);
    bxr_rect_t dst_rect
        = bxr_rect_create(x, y, (float)img_width, (float)img_height);

    bxr_painter_set_image(0, _image);

    bxr_painter_draw_textured_rect(0,
                                   (bxr_textured_rect_t){
                                       .src = src_rect,
                                       .dst = dst_rect,
                                   });
  }
}

void
bxr_game_teardown(void)
{
  bxr_asset_unmount(ASSET_MOUNT);

  bxr_voice_destroy(_music);

  bxr_audio_destroy(_audio);

  bxr_image_destroy(_image);
}
