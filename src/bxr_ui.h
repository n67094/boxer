/**
 * @file bxr_ui.h
 *
 * Root container:
 *
 * Container:
 *
 * Layout:
 *
 * Command:
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_UI_H_
#define BXR_UI_H_

#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_font.h"

typedef enum
{
  BXR_UI_COLOR_TEXT,
  BXR_UI_COLOR_BORDER,
  BXR_UI_COLOR_WINDOWBG,
  BXR_UI_COLOR_TITLEBG,
  BXR_UI_COLOR_TITLETEXT,
  BXR_UI_COLOR_PANELBG,
  BXR_UI_COLOR_BUTTON,
  BXR_UI_COLOR_BUTTONHOVER,
  BXR_UI_COLOR_BUTTONFOCUS,
  BXR_UI_COLOR_BASE,
  BXR_UI_COLOR_BASEHOVER,
  BXR_UI_COLOR_BASEFOCUS,
  BXR_UI_COLOR_SCROLLBASE,
  BXR_UI_COLOR_SCROLLTHUMB,
  BXR_UI_COLOR_MAX
} bxr_ui_color_e;

typedef enum
{
  BXR_UI_OPT_ALIGNCENTER = (1 << 0), // Options for control text and icons (e.g.
                                     // button label). Default is left-aligned.
  BXR_UI_OPT_ALIGNRIGHT = (1 << 1),
  BXR_UI_OPT_NOINTERACT = (1 << 2),
  BXR_UI_OPT_NOFRAME    = (1 << 3),
  BXR_UI_OPT_NORESIZE   = (1 << 4),
  BXR_UI_OPT_NOSCROLL   = (1 << 5),
  BXR_UI_OPT_NOCLOSE    = (1 << 6),
  BXR_UI_OPT_NOTITLE    = (1 << 7),
  BXR_UI_OPT_HOLDFOCUS  = (1 << 8),
  BXR_UI_OPT_AUTOSIZE   = (1 << 9),
  BXR_UI_OPT_POPUP      = (1 << 10),
  BXR_UI_OPT_CLOSED     = (1 << 11),
  BXR_UI_OPT_EXPANDED   = (1 << 12)
} bxr_ui_opt_e;

typedef struct bxr_ui_style_s
{
  bxr_font_t *font;
  bxr_vec2_t size;    // default layout size
  int padding;        // Padding for elements
  int spacing;        // Spacing between layout elements
  int indent;         // Indent for nested tree nodes
  int title_height;   // Title bar height
  int scrollbar_size; // Scrollbar width
  int thumb_size;     // Slider thumb size
  bxr_color_t colors[BXR_UI_COLOR_MAX];
} bxr_ui_style_t;

void bxr_ui_setup(void);

void bxr_ui_shutdown(void);

void bxr_ui_begin(void);

void bxr_ui_end(void);

#endif // BXR_UI_H_
