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

typedef enum
{
  BXR_UI_CLIP_OUT = 0,
  BXR_UI_CLIP_IN,
  BXR_UI_CLIP_PARTIAL,
} bxr_ui_clip_e;

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

/**
 * @brief Begin a new UI frame.
 */
void bxr_ui_begin(void);

/**
 * @brief End the current UI frame and submit draw commands.
 */
void bxr_ui_end(void);

/**
 * @brief Generate a unique ID for a UI element.
 *
 * This use the latest generated ID or a default ID as the base and combine it
 * with the provided data to create a new unique ID.
 *
 * @param data Pointer to the data used to generate the ID (e.g., label,
 * pointer).
 * @param size Size of the data in bytes.
 *
 * @return A unique Uint32 ID for the UI element.
 */
Uint32 bxr_ui_get_id(const void *data, int size);

/**
 * @brief Push a new ID onto the ID stack for nested UI elements.
 *
 * @param data Pointer to the data used to generate the ID (e.g., label,
 * pointer).
 * @param size Size of the data in bytes.
 */
void bxr_ui_push_id(const void *data, int size);

/**
 * @brief Pop the last ID from the ID stack.
 */
void bxr_ui_pop_id(void);

/**
 * @brief Push a new clipping rectangle onto the clip stack.
 *
 * @param rect The new clipping rectangle to push onto the stack.
 */
void bxr_ui_push_clip_rect(bxr_rect_t rect);

/**
 * @brief Pop the last clipping rectangle from the clip stack.
 */
void bxr_ui_pop_clip_rect(void);

/**
 * @brief Get the current clipping rectangle from the top of the clip stack.
 *
 * @return The current clipping rectangle.
 */
bxr_rect_t bxr_ui_get_clip_rect(void);

/**
 * @brief Check if a rectangle is inside, outside, or partially inside the
 * current clipping rectangle.
 *
 * @param rect The rectangle to check against the current clipping rectangle.
 *
 * @return an enum value indicating the clipping status of the rectangle.
 */
bxr_ui_clip_e bxr_ui_check_clip(bxr_rect_t rect);

#endif // BXR_UI_H_
