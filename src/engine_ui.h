#ifndef ENGINE_UI_H_
#define ENGINE_UI_H_

#include "engine_color.h"
#include "engine_config.h"
#include "engine_font.h"

typedef enum
{
  ENGINE_UI_COLOR_TEXT,
  ENGINE_UI_COLOR_BORDER,
  ENGINE_UI_COLOR_WINDOWBG,
  ENGINE_UI_COLOR_TITLEBG,
  ENGINE_UI_COLOR_TITLETEXT,
  ENGINE_UI_COLOR_PANELBG,
  ENGINE_UI_COLOR_BUTTON,
  ENGINE_UI_COLOR_BUTTONHOVER,
  ENGINE_UI_COLOR_BUTTONFOCUS,
  ENGINE_UI_COLOR_BASE,
  ENGINE_UI_COLOR_BASEHOVER,
  ENGINE_UI_COLOR_BASEFOCUS,
  ENGINE_UI_COLOR_SCROLLBASE,
  ENGINE_UI_COLOR_SCROLLTHUMB,
  ENGINE_UI_COLOR_MAX
} engine_ui_color_e;

typedef struct engine_ui_style_s
{
  engine_font_t *font;
  engine_vec2_t size; // default layout size
  int padding;        // Padding for elements
  int spacing;        // Spacing between layout elements
  int indent;         // Indent for nested tree nodes
  int title_height;   // Title bar height
  int scrollbar_size; // Scrollbar width
  int thumb_size;     // Slider thumb size
  engine_color_t colors[ENGINE_UI_COLOR_MAX];
} engine_ui_style_t;

void engine_ui_setup(void);

void engine_ui_shutdown(void);

void engine_ui_begin(void);

void engine_ui_end(void);

#endif // ENGINE_UI_H_
