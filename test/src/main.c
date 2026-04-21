#include "bxr.h"

#include "test_ini.h"
#include "test_json.h"
#include "test_math.h"
// #include "test_text.h"

static bxr_game_config_t _config = { 0 };

bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "boxer_bxr_test";
  _config.title = "Boxer Engine Test";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  _config.target_ups = 60;

  return &_config;
}

void
bxr_game_setup(void)
{
  bxr_unit_enable_colors(true);

  BXR_UNIT_RUN_SUITE(suite_math);
  BXR_UNIT_RUN_SUITE(suite_json);
  BXR_UNIT_RUN_SUITE(suite_ini);
  // BXR_UNIT_RUN_SUITE(suite_text);
}

void
bxr_game_update(Uint64 delta_time_ms)
{
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
}

void
bxr_game_shutdown(void)
{
}
