#include "engine.h"

#include "test_font.h"
#include "test_image.h"
#include "test_ini.h"
#include "test_json.h"
#include "test_math.h"

typedef enum
{
  TEST_IMAGE = 0,
  TEST_FONT,
  TEST_SIZE,
} test_e;

static test_e _current_test = TEST_IMAGE;

static engine_game_config_t _config = { 0 };

engine_game_config_t *
engine_game_config(void)
{
  _config.name  = "boxer_engine_test";
  _config.title = "Boxer Engine Test";

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
  engine_unit_enable_colors(true);

  ENGINE_UNIT_RUN_SUITE(suite_math);
  ENGINE_UNIT_RUN_SUITE(suite_json);
  ENGINE_UNIT_RUN_SUITE(suite_ini);

  test_image_setup();
  test_font_setup();
}

void
engine_game_update(Uint64 delta_time_ms)
{
  // Cycle test left
  if (engine_key_just_pressed(ENGINE_KEY_LEFT)
      || engine_gamepad_just_pressed(0, ENGINE_GAMEPAD_BUTTON_DPAD_LEFT)
      || engine_mouse_just_pressed(ENGINE_MOUSE_LEFT)) {
    SDL_Log("Current test: %d", _current_test);
    _current_test = (_current_test + TEST_SIZE - 1) % (TEST_SIZE);
  }

  // Cycle test right
  if (engine_key_just_pressed(ENGINE_KEY_RIGHT)
      || engine_gamepad_just_pressed(0, ENGINE_GAMEPAD_BUTTON_DPAD_RIGHT)
      || engine_mouse_just_pressed(ENGINE_MOUSE_RIGHT)) {
    SDL_Log("Current test: %d", _current_test);
    _current_test = (_current_test + 1) % (TEST_SIZE);
  }

  if (engine_key_held(ENGINE_KEY_SPACE)
      || engine_gamepad_held(0, ENGINE_GAMEPAD_BUTTON_SOUTH)) {
    SDL_Log("pressed south");
  }

  if (engine_key_held(ENGINE_KEY_RETURN)
      || engine_gamepad_held(0, ENGINE_GAMEPAD_BUTTON_NORTH)) {
    SDL_Log("pressed north");
  }

  switch (_current_test) {
  case TEST_IMAGE:
    test_image_update(delta_time_ms);
    break;
  case TEST_FONT:
    test_font_update(delta_time_ms);
    break;
  default:
    break;
  }
}

void
engine_game_render(Uint64 alpha_time_ms)
{
  engine_vec2_t window_dimensions = engine_context_get_window_dimensions();

  engine_painter_begin(window_dimensions.x / 2,
                       window_dimensions.y / 2); // zoom x2
  {
    switch (_current_test) {
    case TEST_IMAGE:
      test_image_render(alpha_time_ms);
      break;
    case TEST_FONT:
      test_font_render(alpha_time_ms);
      break;
    default:
      break;
    }

    engine_painter_flush();
  }
  engine_painter_end();
}

void
engine_game_shutdown(void)
{
  test_image_shutdown();
  test_font_shutdown();
}
