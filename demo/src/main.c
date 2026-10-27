#include <boxer/bxr.h>

#include "defs.h"
#include "scene.h"

static bxr_game_config_t _config = { 0 };

const bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "boxer_demo";
  _config.title = "Skullboy (Boxer Demo)";

  _config.width  = WINDOW_WIDTH;
  _config.height = WINDOW_HEIGHT;

  _config.virtual_width  = WINDOW_VIRTUAL_WIDTH;
  _config.virtual_height = WINDOW_VIRTUAL_HEIGHT;

  _config.fullscreen = false;
  _config.resizable  = false;

  return &_config;
}

void
bxr_game_setup(void)
{
  // Mount the asset archive to the virtual file system
  bxr_asset_mount(ASSET_MOUNT);

  bxr_log_enable_colors(true);

  scene_go_to(SCENE_MENU);
}

void
bxr_game_update(double delta_ms)
{
  scene_update(delta_ms);
}

void
bxr_game_render(double alpha_ms)
{
  // Clear the screen to black before rendering the game scene
  bxr_painter_set_color(BXR_COLOR_BLACK);
  bxr_painter_clear();

  scene_render(alpha_ms);
}

void
bxr_game_teardown(void)
{
  scene_teardown();

  bxr_asset_unmount(ASSET_MOUNT);
}
