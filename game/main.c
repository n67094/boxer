#include <bxr.h>

static bxr_image_t _image;

static bxr_game_config_t _config = { 0 };

bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "boxer_bxr";
  _config.title = "Boxer Engine";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  return &_config;
}

void
bxr_game_setup(void)
{
  bxr_asset_mount("data.zip");

  // Load an image
  _image = bxr_image_create("data/images/logo.png");
}

void
bxr_game_update(Uint64 delta_time_ms)
{
  // Update game logic here
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
  bxr_vec2_t window_size = bxr_context_get_window_dimensions();

  bxr_painter_begin(window_size.x, window_size.y);
  {
    bxr_painter_set_color(bxr_color_create(0, 0, 0, 255));
    bxr_painter_clear();

    float width  = window_size.x;
    float height = window_size.y;

    int img_width  = bxr_image_get_width(_image);
    int img_height = bxr_image_get_height(_image);

    bxr_painter_push_transform();
    {
      // Move the origin to the center of the screen
      // bxr_painter_translate(width * 0.5f, height * 0.5f);

      // Scale the image by 2
      bxr_painter_scale(2.0f, 2.0f);

      bxr_painter_set_color(bxr_color_create(255, 255, 255, 255));

      for (int i = 0; i < 4096; ++i) {
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

      /*
     int width  = bxr_image_get_width(_image);
     int height = bxr_image_get_height(_image);

     bxr_rect_t img_src = bxr_rect_create(0, 0, (float)width, (float)height);
     bxr_rect_t img_dst = bxr_rect_create(
         -width * 0.5f, -height * 0.5f, (float)width, (float)height);

     // Draw the image centered on the screen
     bxr_painter_set_image(0, _image);
     bxr_painter_draw_textured_rect(
         0, bxr_textured_rect_create(img_dst, img_src));
         */
    }
    bxr_painter_pop_transform();

    bxr_painter_flush();
  }
  bxr_painter_end();
}

void
bxr_game_shutdown(void)
{
  bxr_asset_unmount("data.zip");

  bxr_image_destroy(_image);
}
