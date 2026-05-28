#include <boxer/bxr.h>

#define ASSET_MOUNT "assets.zip"
#define ASSET_PATH "assets/"

static bxr_image_t _image;

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

  // Load an image
  _image = bxr_image_create(ASSET_PATH "images/logo.png");
}

void
bxr_game_update(Uint64 delta_time_ms)
{
  // Update game logic here
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
  bxr_vec2_t window_size = bxr_context_get_virtual_window_dimensions();

  bxr_painter_set_color(bxr_color_create(0, 0, 0, 255));
  bxr_painter_clear();

  for (int i = 0; i < 4096; ++i) {
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

  bxr_image_destroy(_image);
}
