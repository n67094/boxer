/**
 * @file bxr_imui.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_IMUI_H_
#define BXR_IMUI_H_

typedef enum
{
  IMUI_COLOR_TEXT,
  IMUI_COLOR_BORDER,
  IMUI_COLOR_WINDOWBG,
  IMUI_COLOR_TITLEBG,
  IMUI_COLOR_TITLETEXT,
  IMUI_COLOR_PANELBG,
  IMUI_COLOR_BUTTON,
  IMUI_COLOR_BUTTONHOVER,
  IMUI_COLOR_BUTTONFOCUS,
  IMUI_COLOR_BASE,
  IMUI_COLOR_BASEHOVER,
  IMUI_COLOR_BASEFOCUS,
  IMUI_COLOR_SCROLLBASE,
  IMUI_COLOR_SCROLLTHUMB,
  IMUI_COLOR_MAX
} bxr_imui_color_e;

typedef enum
{
  IMUI_ICON_CLOSE = 1,
  IMUI_ICON_CHECK,
  IMUI_ICON_COLLAPSED,
  IMUI_ICON_EXPANDED,
  IMUI_ICON_MAX
} bxr_imui_icon_e;

typedef enum
{
  IMUI_OPT_ALIGNCENTER = (1 << 0),
  IMUI_OPT_ALIGNRIGHT  = (1 << 1),
  IMUI_OPT_NOINTERACT  = (1 << 2),
  IMUI_OPT_NOFRAME     = (1 << 3),
  IMUI_OPT_NORESIZE    = (1 << 4),
  IMUI_OPT_NOSCROLL    = (1 << 5),
  IMUI_OPT_NOCLOSE     = (1 << 6),
  IMUI_OPT_NOTITLE     = (1 << 7),
  IMUI_OPT_HOLDFOCUS   = (1 << 8),
  IMUI_OPT_AUTOSIZE    = (1 << 9),
  IMUI_OPT_POPUP       = (1 << 10),
  IMUI_OPT_CLOSED      = (1 << 11),
  IMUI_OPT_EXPANDED    = (1 << 12)
} bxr_imui_opt_e;

typedef struct bxr_imui_font_s
{
  // TODO
} bxr_imui_font_t;

#endif // BXR_IMUI_H_
