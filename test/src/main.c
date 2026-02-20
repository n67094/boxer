#include "engine.h"

#include "test_ini.h"
#include "test_json.h"
#include "test_math.h"

static engine_game_config_t _config = { 0 };

engine_game_config_t *
engine_game_config(void)
{
  _config.name  = "boxer_engine_test";
  _config.title = "Boxer Engine Test";

  _config.width  = 800;
  _config.height = 600;

  _config.fullscreen = false;
  _config.resizable  = false;

  _config.target_ups = 60;

  return &_config;
}

void
engine_game_setup(void)
{
  engine_unit_enable_colors(true);

  ENGINE_UNIT_RUN_SUITE(suite_math);
  ENGINE_UNIT_RUN_SUITE(suite_json);
  ENGINE_UNIT_RUN_SUITE(suite_ini);
}

void
engine_game_update(Uint64 delta_time_ms)
{
}

void
engine_game_render(Uint64 alpha_time_ms)
{
}

void
engine_game_shutdown(void)
{
}
