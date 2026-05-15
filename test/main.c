#include "bxr.h"

#include "ecs.h"
#include "ini.h"
#include "json.h"
#include "math.h"
#include "sparse_set.h"

static bxr_game_config_t _config = { 0 };

bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "boxer_engine_test";
  _config.title = "Boxer Engine Test";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  return &_config;
}

void
bxr_game_setup(void)
{
  bxr_unit_enable_colors(true);

  BXR_UNIT_RUN_SUITE(suite_math);
  BXR_UNIT_RUN_SUITE(suite_json);
  BXR_UNIT_RUN_SUITE(suite_ini);
  BXR_UNIT_RUN_SUITE(suite_sparse_set);
  BXR_UNIT_RUN_SUITE(suite_ecs);

  bxr_unit_report();
}

void
bxr_game_update(Uint64 delta_time_ms)
{
  bxr_quit();
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
}

void
bxr_game_shutdown(void)
{
}
