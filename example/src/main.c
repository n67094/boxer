#include <boxer/bxr.h>

#include "defs.h"
#include "level.h"
#include "menu.h"
#include "state.h"

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

  // Setup the menu and level systems at once for simplicity.
  state_setup();
  menu_setup();
  level_setup();

  // Set the initial game state to the menu
  state_set(STATE_MENU);
}

void
bxr_game_update(double delta_time_ms)
{

  switch (state_get()) {
  case STATE_MENU:
    menu_update(delta_time_ms);
    break;
  case STATE_LEVEL:
    level_update(delta_time_ms);
    break;
  default:
    break;
  }
}

void
bxr_game_render(double alpha)
{
  // Clear the screen to black before rendering the game scene
  bxr_painter_set_color(BXR_COLOR_BLACK);
  bxr_painter_clear();

  switch (state_get()) {
  case STATE_MENU:
    menu_render(alpha);
    break;
  case STATE_LEVEL:
    level_render(alpha);
    break;
  default:
    break;
  }
}

void
bxr_game_teardown(void)
{
  menu_teardown();
  level_teardown();
  state_teardown();

  bxr_asset_unmount(ASSET_MOUNT);
}
