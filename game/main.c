#include <engine.h>

static engine_image_t _image;

static engine_game_config_t _config = { 0 };

engine_game_config_t *
engine_game_config(void)
{
  _config.name  = "boxer_engine";
  _config.title = "Boxer Engine";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  _config.target_ups = 60;

  return &_config;
}

void
engine_game_setup(void)
{
  // Load an image
  _image = engine_image_load("data/images/logo.png");
}

void
engine_game_update(Uint64 delta_time_ms)
{
  // Update game logic here
}

void
engine_game_render(Uint64 alpha_time_ms)
{
  engine_painter_begin();
  {
    engine_vec2_t frame_size = engine_painter_get_frame_size();

    float width  = frame_size.x;
    float height = frame_size.y;

    engine_painter_push_transform();
    {
      // Move the origin to the center of the screen
      engine_painter_translate(width * 0.5f, height * 0.5f);

      // Scale the image by 2
      engine_painter_scale(2.0f, 2.0f);

      engine_painter_set_color(engine_color_make(255, 255, 255, 255));

      int width  = engine_image_get_width(_image);
      int height = engine_image_get_height(_image);

      engine_rect_t img_src
          = engine_rect_make(0, 0, (float)width, (float)height);
      engine_rect_t img_dst = engine_rect_make(
          -width * 0.5f, -height * 0.5f, (float)width, (float)height);

      // Draw the image centered on the screen
      engine_painter_set_image(_image);
      engine_painter_draw_rect_textured(
          engine_textured_rect_make(img_dst, img_src));
    }
    engine_painter_pop_transform();

    engine_painter_flush();
  }
  engine_painter_end();
}

void
engine_game_shutdown(void)
{
  engine_image_destroy(_image);
}
