#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_config.h"
#include "engine_font.h"
#include "engine_ui.h"

typedef struct _engine_ui_command_s
{
} _engine_ui_command_t;

typedef struct _engine_ui_layout_s
{
} _engine_ui_layout_t;

typedef struct _engine_ui_container_s
{
} _engine_ui_container_t;

typedef struct _engine_ui_state_s
{
  engine_ui_style_t _style; // default style
  engine_ui_style_t *style; // current style, can be modified

  Uint32 current_container;
} _engine_ui_state_t;

typedef struct _engine_ui_s
{
  // States stack
  Uint32 current_state;
  _engine_ui_state_t states[ENGINE_UI_MAX]; // save states for push/pop
  _engine_ui_state_t state;                 // current state

  // Roots stack
  Uint32 current_root;
  _engine_ui_container_t roots[ENGINE_UI_ROOT_MAX]; // save roots for push/pop
} _engine_ui_t;

static engine_ui_style_t default_style = {
  NULL,       // font
  { 68, 10 }, // size
  5,          // padding
  4,          // spacing
  24,         // indent
  24,         // title_height
  12,         // scrollbar_size
  8,          // thumb_size
  {
      { 230, 230, 230, 255 }, /* ENGINE_UI_COLOR_TEXT */
      { 25, 25, 25, 255 },    /* ENGINE_UI_COLOR_BORDER */
      { 50, 50, 50, 255 },    /* ENGINE_UI_COLOR_WINDOWBG */
      { 25, 25, 25, 255 },    /* ENGINE_UI_COLOR_TITLEBG */
      { 240, 240, 240, 255 }, /* ENGINE_UI_COLOR_TITLETEXT */
      { 0, 0, 0, 0 },         /* ENGINE_UI_COLOR_PANELBG */
      { 75, 75, 75, 255 },    /* ENGINE_UI_COLOR_BUTTON */
      { 95, 95, 95, 255 },    /* ENGINE_UI_COLOR_BUTTONHOVER */
      { 115, 115, 115, 255 }, /* ENGINE_UI_COLOR_BUTTONFOCUS */
      { 30, 30, 30, 255 },    /* ENGINE_UI_COLOR_BASE */
      { 35, 35, 35, 255 },    /* ENGINE_UI_COLOR_BASEHOVER */
      { 40, 40, 40, 255 },    /* ENGINE_UI_COLOR_BASEFOCUS */
      { 43, 43, 43, 255 },    /* ENGINE_UI_COLOR_SCROLLBASE */
      { 30, 30, 30, 255 },    /* ENGINE_UI_COLOR_SCROLLTHUMB */
  },
};

static _engine_ui_t _ui    = { 0 };
static Uint32 _initialized = 0;

void
engine_ui_setup(void)
{
  SDL_assert(_initialized == 0);
  _initialized = ENGINE_INIT_COOKIE;
}

void
engine_ui_shutdown(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  _initialized = 0;
}

void
engine_ui_begin(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_ui.current_state < ENGINE_UI_MAX);
}

void
engine_ui_end(void)
{
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
}
