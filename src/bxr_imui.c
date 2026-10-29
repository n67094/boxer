#include "bxr_imui.h"
#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_math.h"

typedef enum
{
  IMUI_COMMAND_JUMP = 1,
  IMUI_COMMAND_CLIP,
  IMUI_COMMAND_RECT,
  IMUI_COMMAND_TEXT,
  IMUI_COMMAND_ICON,
  IMUI_COMMAND_SIZE
} bxr_imui_command_e_;

typedef struct imui_command_base_s_
{
  bxr_imui_command_e_ type;
  size_t size;
} imui_command_base_t_;

typedef struct imui_jump_command_s_
{
  imui_command_base_t_ base;
  void *dst;
} imui_jump_command_t_;

typedef struct imui_clip_command_s_
{
  imui_command_base_t_ base;
  bxr_rect_t rect;
} imui_clip_command_t_;

typedef struct imui_rect_command_s_
{
  imui_command_base_t_ base;
  bxr_rect_t rect;
  bxr_color_t color;
} imui_rect_command_t_;

typedef struct imui_text_command_s_
{
  imui_command_base_t_ base;
  bxr_imui_font_t font;
  bxr_vec2_t pos;
  bxr_color_t color;
  char str[BXR_IMUI_STR_MAX];
} imui_text_command_t_;

typedef struct imui_icon_command_s_
{
  imui_command_base_t_ base;
  bxr_rect_t rect;
  bxr_imui_icon_e icon;
  bxr_vec2_t pos;
  bxr_color_t color;
} imui_icon_command_t_;

typedef union
{
  int type;
  imui_command_base_t_ base;
  imui_jump_command_t_ jump;
  imui_clip_command_t_ clip;
  imui_rect_command_t_ rect;
  imui_text_command_t_ text;
  imui_icon_command_t_ icon;
} tui_command_t_;
