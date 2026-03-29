#include "bxr.h"

#include "example_blend.h"
#include "example_draw.h"
#include "example_text.h"

typedef enum
{
  EXAMPLE_DRAW = 0,
  // EXAMPLE_FONT,
  EXAMPLE_BLEND,
  EXAMPLE_SIZE,
} example_e;

static example_e _current_test = EXAMPLE_DRAW;

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

  example_draw_setup();
  // example_text_setup();
  example_blend_setup();
}

void
bxr_game_update(Uint64 delta_time_ms)
{
  // Cycle example left
  if (bxr_key_just_pressed(BXR_KEY_LEFT)
      || bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_LEFT)
      || bxr_mouse_just_pressed(BXR_MOUSE_LEFT)) {
    _current_test = (_current_test + EXAMPLE_SIZE - 1) % (EXAMPLE_SIZE);
  }

  // Cycle example right
  if (bxr_key_just_pressed(BXR_KEY_RIGHT)
      || bxr_gamepad_just_pressed(0, BXR_GAMEPAD_BUTTON_DPAD_RIGHT)
      || bxr_mouse_just_pressed(BXR_MOUSE_RIGHT)) {
    _current_test = (_current_test + 1) % (EXAMPLE_SIZE);
  }

  switch (_current_test) {
  case EXAMPLE_DRAW:
    example_draw_update(delta_time_ms);
    break;
  // case EXAMPLE_FONT:
  // example_text_update(delta_time_ms);
  // break;
  case EXAMPLE_BLEND:
    example_blend_update(delta_time_ms);
    break;
  default:
    break;
  }
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
  bxr_painter_begin();
  {
    switch (_current_test) {
    case EXAMPLE_DRAW:
      example_draw_render(alpha_time_ms);
      break;
      // case EXAMPLE_FONT:
      // example_text_render(alpha_time_ms);
      // break;
    case EXAMPLE_BLEND:
      example_blend_render(alpha_time_ms);
    default:
      break;
    }

    bxr_painter_flush();
  }
  bxr_painter_end();
}

void
bxr_game_shutdown(void)
{
  example_draw_shutdown();
  // example_text_shutdown();
  example_blend_shutdown();
}
