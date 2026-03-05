#include "engine.h"

#include "example_image.h"
#include "example_text.h"

#include "test_ini.h"
#include "test_json.h"
#include "test_math.h"
#include "test_text.h"

typedef enum
{
  EXAMPLE_IMAGE = 0,
  EXAMPLE_FONT,
  EXAMPLE_SIZE,
} example_e;

static example_e _current_test = EXAMPLE_IMAGE;

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
  ENGINE_UNIT_RUN_SUITE(suite_text);

  example_image_setup();
  example_text_setup();
}

void
engine_game_update(Uint64 delta_time_ms)
{
  // Cycle example left
  if (engine_key_just_pressed(ENGINE_KEY_LEFT)
      || engine_gamepad_just_pressed(0, ENGINE_GAMEPAD_BUTTON_DPAD_LEFT)
      || engine_mouse_just_pressed(ENGINE_MOUSE_LEFT)) {
    _current_test = (_current_test + EXAMPLE_SIZE - 1) % (EXAMPLE_SIZE);
  }

  // Cycle example right
  if (engine_key_just_pressed(ENGINE_KEY_RIGHT)
      || engine_gamepad_just_pressed(0, ENGINE_GAMEPAD_BUTTON_DPAD_RIGHT)
      || engine_mouse_just_pressed(ENGINE_MOUSE_RIGHT)) {
    _current_test = (_current_test + 1) % (EXAMPLE_SIZE);
  }

  switch (_current_test) {
  case EXAMPLE_IMAGE:
    example_image_update(delta_time_ms);
    break;
  case EXAMPLE_FONT:
    example_text_update(delta_time_ms);
    break;
  default:
    break;
  }
}

void
engine_game_render(Uint64 alpha_time_ms)
{
  engine_vec2_t window_dimensions = engine_context_get_window_dimensions();

  engine_painter_begin(window_dimensions.x * 0.5f,
                       window_dimensions.y * 0.5); // zoom x2
  {
    switch (_current_test) {
    case EXAMPLE_IMAGE:
      example_image_render(alpha_time_ms);
      break;
    case EXAMPLE_FONT:
      example_text_render(alpha_time_ms);
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
  example_image_shutdown();
  example_text_shutdown();
}
