#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_font.h"
#include "bxr_math.h"
#include "bxr_ui.h"

typedef enum
{
  BXR_UI_COMMAND_JUMP = 1,
  BXR_UI_COMMAND_CLIP,
  BXR_UI_COMMAND_RECT,
  BXR_UI_COMMAND_TEXT,
  BXR_UI_COMMAND_SIZE
} _bxr_ui_command_e;

typedef struct _bxr_ui_base_command_s
{
  int type, size;
} _bxr_ui_base_command_t;

typedef struct _bxr_ui_jump_command_s
{
  _bxr_ui_base_command_t base;
  void *dst;
} _bxr_ui_jump_command_t;

typedef struct
{
  _bxr_ui_base_command_t base;
  bxr_rect_t rect;
} _bxr_ui_clip_command_t;

typedef struct _bxr_ui_rect_command_s
{
  _bxr_ui_base_command_t base;
  bxr_rect_t rect;
  bxr_color_t color;
} _bxr_ui_rect_command_t;

typedef struct _bxr_ui_text_command_s
{
  _bxr_ui_base_command_t base;
  bxr_font_t *font;
  bxr_rect_t pos;
  bxr_color_t color;
  char str[1];
} _bxr_ui_text_command_t;

typedef union _bxr_ui_command_u
{
  int type;
  _bxr_ui_base_command_t base;
  _bxr_ui_jump_command_t jump;
  _bxr_ui_clip_command_t clip;
  _bxr_ui_rect_command_t rect;
  _bxr_ui_text_command_t text;
} _bxr_ui_command_t;

typedef struct _bxr_ui_layout_s
{
  bxr_rect_t body;   // Bounds
  bxr_rect_t next;   // Next element position
  bxr_vec2_t cursor; // Cursor position for current row
  bxr_vec2_t size;   // Size of the layout
  bxr_vec2_t max;
  int widths[BXR_UI_LAYOUT_SLOTS_MAX]; // Widths of the columns
  int items;                           // Number of items in the row
  int item_index;                      // Index of the next item in the row
  int next_row_y;                      // Y position of the next row
  int next_type; // Type of the next layout (none, relative, absolute)
  int indent;
} _bxr_ui_layout_t;

typedef struct _bxr_ui_container_s
{
  _bxr_ui_command_t *head, *tail; // Commands
  bxr_rect_t rect;
  bxr_rect_t body;
  bxr_vec2_t content_size;
  bxr_vec2_t scroll;
  int zindex;
  int open;
} _bxr_ui_container_t;

typedef struct _bxr_ui_state_s
{
  bxr_ui_style_t _style; // default style
  bxr_ui_style_t *style; // current style, can be modified

  // Ids stack
  Uint32 current_id;
  bxr_rect_t ids[BXR_UI_CLIP_MAX];

  // Command stack
  Uint32 current_command;
  _bxr_ui_command_t commands[BXR_UI_COMMAND_MAX];

  // Roots stack
  Uint32 current_root;
  _bxr_ui_container_t roots[BXR_UI_ROOT_MAX];

  // Clip stack
  Uint32 current_clip;
  bxr_rect_t clips[BXR_UI_CLIP_MAX];

  // Layout stack
  Uint32 current_layout;
  _bxr_ui_layout_t layouts[BXR_UI_STATE_MAX];
} _bxr_ui_state_t;

typedef struct _bxr_ui_s
{
  // States stack
  Uint32 current_state;
  _bxr_ui_state_t states[BXR_UI_STATE_MAX];
  _bxr_ui_state_t state; // current state
} _bxr_ui_t;

static bxr_ui_style_t default_style = {
  NULL,       // font
  { 68, 10 }, // size
  5,          // padding
  4,          // spacing
  24,         // indent
  24,         // title_height
  12,         // scrollbar_size
  8,          // thumb_size
  {
      { 230, 230, 230, 255 }, /* BXR_UI_COLOR_TEXT */
      { 25, 25, 25, 255 },    /* BXR_UI_COLOR_BORDER */
      { 50, 50, 50, 255 },    /* BXR_UI_COLOR_WINDOWBG */
      { 25, 25, 25, 255 },    /* BXR_UI_COLOR_TITLEBG */
      { 240, 240, 240, 255 }, /* BXR_UI_COLOR_TITLETEXT */
      { 0, 0, 0, 0 },         /* BXR_UI_COLOR_PANELBG */
      { 75, 75, 75, 255 },    /* BXR_UI_COLOR_BUTTON */
      { 95, 95, 95, 255 },    /* BXR_UI_COLOR_BUTTONHOVER */
      { 115, 115, 115, 255 }, /* BXR_UI_COLOR_BUTTONFOCUS */
      { 30, 30, 30, 255 },    /* BXR_UI_COLOR_BASE */
      { 35, 35, 35, 255 },    /* BXR_UI_COLOR_BASEHOVER */
      { 40, 40, 40, 255 },    /* BXR_UI_COLOR_BASEFOCUS */
      { 43, 43, 43, 255 },    /* BXR_UI_COLOR_SCROLLBASE */
      { 30, 30, 30, 255 },    /* BXR_UI_COLOR_SCROLLTHUMB */
  },
};

static _bxr_ui_t _ui       = { 0 };
static Uint32 _initialized = 0;

void
bxr_ui_setup(void)
{
  SDL_assert(_initialized == 0);
  _initialized = BXR_INIT_COOKIE;
}

void
bxr_ui_shutdown(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  _initialized = 0;
}

void
bxr_ui_begin()
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state < BXR_UI_STATE_MAX);
}

void
bxr_ui_end(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
}
