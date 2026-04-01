#include <SDL3/SDL.h>

#include "bxr_atom.h"
#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_font.h"
#include "bxr_math.h"
#include "bxr_mouse.h"
#include "bxr_painter.h"
#include "bxr_text.h"
#include "bxr_ui.h"

// TODO for controller and keyboard navigation support store a stack of
// focusable containers. So the stack can be iterate forward and backward to
// find the next focusable container. ?

typedef enum
{
  BXR_UI_LAYOUT_RELATIVE = 1,
  BXR_UI_LAYOUT_ABSOLUTE = 2
} _bxr_ui_layout_e;

typedef enum
{
  BXR_UI_COMMAND_JUMP = 1,
  BXR_UI_COMMAND_CLIP,
  BXR_UI_COMMAND_RECT,
  BXR_UI_COMMAND_TEXT,
  BXR_UI_COMMAND_ICON,
  BXR_UI_COMMAND_SIZE
} _bxr_ui_command_e;

typedef struct _bxr_ui_style_s
{
  bxr_font_t *font;
  bxr_vec2_t layout_dimensions; // Default layout dimensions
  int padding;                  // Padding for elements
  int spacing;                  // Spacing between layout elements
  int indent;                   // Indent for nested tree nodes
  int title_height;             // Title bar height
  int scrollbar_with;           // Scrollbar width
  int thumb_width;              // Slider thumb width
  bxr_color_t colors[BXR_UI_COLOR_MAX];
} _bxr_ui_style_t;

typedef struct _bxr_ui_base_command_s
{
  int type;
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
  bxr_vec2_t pos;
  bxr_color_t color;
  const char *str;
} _bxr_ui_text_command_t;

typedef struct _bxr_ui_icon_command_s
{
  _bxr_ui_base_command_t base;
  bxr_font_t *font;
  bxr_rect_t rect;
  int icon_index;
} _bxr_ui_icon_command_t;

typedef union _bxr_ui_command_args_u
{
  _bxr_ui_base_command_t base;
  _bxr_ui_jump_command_t jump;
  _bxr_ui_clip_command_t clip;
  _bxr_ui_rect_command_t rect;
  _bxr_ui_text_command_t text;
  _bxr_ui_icon_command_t icon;

} _bxr_ui_command_args_t;

typedef struct _bxr_ui_command_s
{
  _bxr_ui_command_e type;
  _bxr_ui_command_args_t args;
} _bxr_ui_command_t;

typedef struct _bxr_ui_pool_item_s
{
  Uint32 id;
  int last_update;
} _bxr_ui_pool_item_t;

typedef struct _bxr_ui_layout_s
{
  bxr_rect_t body;   // Bounds
  bxr_rect_t next;   // Set by bxr_ui_layout_set_next
  bxr_vec2_t cursor; // Cursor position for current row
  bxr_vec2_t dimensions;
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
  _bxr_ui_command_t *head, *tail; // Commands linked list
  bxr_rect_t rect;
  bxr_rect_t body;
  bxr_vec2_t content_size;
  bxr_vec2_t scroll;
  int zindex;
  int open;
} _bxr_ui_container_t;

typedef struct _bxr_ui_state_s
{
  _bxr_ui_style_t _style;
  _bxr_ui_style_t *style; // style pointer for modification

  // Container state
  bxr_ui_id_t focused_id;
  bxr_ui_id_t hovered_id;

  // Whether the focus was updated in the current frame.
  bool update_focus;

  bxr_rect_t viewport;

  bxr_ui_id_t last_id; // last generated ID
  int last_zindex;

  // The hovered root container
  _bxr_ui_container_t *hovered_root;

  // TODO missing members

  // Ids stack
  Uint32 current_id;
  Uint32 ids[BXR_UI_CLIP_MAX];

  // Command stack
  Uint32 current_command;
  _bxr_ui_command_t commands[BXR_UI_COMMAND_MAX];

  // Roots stack
  Uint32 current_root;
  _bxr_ui_container_t roots[BXR_UI_ROOT_MAX];

  // Containers stack
  Uint32 current_container;
  _bxr_ui_container_t *containers[BXR_UI_CONTAINER_SLOTS_MAX];

  // Layout stack
  Uint32 current_layout;
  _bxr_ui_layout_t layouts[BXR_UI_STATE_MAX];

  // Clip stack
  Uint32 current_clip;
  bxr_rect_t clips[BXR_UI_CLIP_MAX];

  // Containers data
  _bxr_ui_container_t containers_data[BXR_UI_CONTAINER_SLOTS_MAX];

  _bxr_ui_pool_item_t container_pool[BXR_UI_CONTAINER_SLOTS_MAX];

  _bxr_ui_pool_item_t treenode_pool[BXR_UI_CONTAINER_SLOTS_MAX];

  // TODO missing members

  bxr_vec2_t mouse_delta;
  bxr_vec2_t scroll_delta;

  char text_input[32];

  int frame;
} _bxr_ui_state_t;

typedef struct _bxr_ui_s
{
  // States stack
  Uint32 current_state;
  _bxr_ui_state_t states[BXR_UI_STATE_MAX];
  _bxr_ui_state_t state; // current state
} _bxr_ui_t;

static _bxr_ui_style_t default_style = {
  NULL,       // Font
  { 68, 10 }, // Default layout demensions
  5,          // Padding
  4,          // Spacing
  24,         // Indent
  24,         // Title height
  12,         // Scrollbar width
  8,          // Slider thumb width
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

// Update the frame at items[idx] to the current frame.
static void
_bxr_ui_pool_update(_bxr_ui_pool_item_t *items, int idx)
{
  items[idx].last_update = _ui.state.frame;
}

// Init a pool item with the given ID.
static int
_bxr_ui_pool_init(_bxr_ui_pool_item_t *items, int len, Uint32 id)
{
  int i = 0;
  int n = -1;
  int f = _ui.state.frame;

  // Loop through the pool to find the item with the oldest last_update value,
  // which indicates that it is the least recently used item. This item will be
  // replaced with the new ID.
  for (i = 0; i < len; i++) {
    if (items[i].last_update < f) {
      f = items[i].last_update;
      n = i;
    }
  }

  SDL_assert(n > -1);
  items[n].id = id;

  _bxr_ui_pool_update(items, n);

  return n;
}

// Iterate through the pool to find the item with the given ID and return its
// index, or -1 if not found.
static int
_bxr_ui_pool_get(_bxr_ui_pool_item_t *items, int len, Uint32 id)
{
  int i;

  for (i = 0; i < len; i++) {
    if (items[i].id == id) {
      return i;
    }
  }
  return -1;
}

// Push a new command to the command list and return a pointer to it.
static _bxr_ui_command_t *
_bxr_ui_command_push(int type)
{
  SDL_assert(_ui.state.current_command < BXR_UI_COMMAND_MAX);
  _bxr_ui_command_t *cmd = _ui.state.commands + _ui.state.current_command;

  cmd->args.base.type = type;

  _ui.state.current_command++;

  return cmd;
}

// Iterate through the command list and move the pointer to the next command
// that is not a jump command. Return true if such a command is found, false
// otherwise.
static bool
_bxr_ui_command_next(_bxr_ui_command_t **cmd)
{
  if (*cmd) {
    // cmd is not NULL move to the next command
    *cmd = (*cmd) + 1;
  } else {
    // cmd is NULL, start from the first command
    *cmd = _ui.state.commands;
  }

  while (*cmd != _ui.state.commands + _ui.state.current_command) {
    if ((*cmd)->args.base.type != BXR_UI_COMMAND_JUMP) {
      return true;
    }

    *cmd = (*cmd)->args.jump.dst;
  }

  return false;
}

// Push a jump command to the command list and return a pointer to it.
static _bxr_ui_command_t *
_bxr_ui_command_jump(_bxr_ui_command_t *dst)
{
  _bxr_ui_command_t *cmd = _bxr_ui_command_push(BXR_UI_COMMAND_JUMP);
  cmd->args.jump.dst     = dst;
  return cmd;
}

// Set the current clipping rectangle by pushing a new clip command.
static void
_bxr_ui_command_clip(bxr_rect_t rect)
{
  _bxr_ui_command_t *cmd;
  cmd                 = _bxr_ui_command_push(BXR_UI_COMMAND_CLIP);
  cmd->args.clip.rect = rect;
}

// Draw a filled rectangle by pushing a new rect command.
static void
_bxr_ui_command_rect(bxr_rect_t rect, bxr_color_t color)
{
  _bxr_ui_command_t *cmd;

  rect = bxr_rect_get_intersection(rect, bxr_ui_clip_rect_get());

  if (rect.w > 0 && rect.h > 0) {
    cmd                  = _bxr_ui_command_push(BXR_UI_COMMAND_RECT);
    cmd->args.rect.rect  = rect;
    cmd->args.rect.color = color;
  }
}

// Draw a border by drawing four rectangles around the edges of the given
// rectangle.
static void
_bxr_ui_command_box(bxr_rect_t rect, bxr_color_t color)
{
  _bxr_ui_command_rect(bxr_rect_make(rect.x + 1, rect.y, rect.w - 2, 1), color);
  _bxr_ui_command_rect(
      bxr_rect_make(rect.x + 1, rect.y + rect.h - 1, rect.w - 2, 1), color);
  _bxr_ui_command_rect(bxr_rect_make(rect.x, rect.y, 1, rect.h), color);
  _bxr_ui_command_rect(bxr_rect_make(rect.x + rect.w - 1, rect.y, 1, rect.h),
                       color);
}

// Draw text by pushing a new text command.
static void
_bxr_ui_command_text(bxr_font_t *font,
                     const char *str,
                     int len,
                     bxr_vec2_t pos,
                     bxr_color_t color)
{
  if (str == NULL || *str == '\0') {
    return;
  }

  _bxr_ui_command_t *cmd;

  bxr_vec2_t text_size = bxr_text_measure(font, str);
  bxr_rect_t rect      = bxr_rect_make(pos.x, pos.y, text_size.x, text_size.y);

  bxr_ui_clip_e clipped = bxr_ui_check_clip(rect);

  // If the text is fully outside the clipping rectangle, skip drawing it.
  if (clipped == BXR_UI_CLIP_OUT) {
    return;
  }

  // If the text is partially inside the clipping rectangle, set the clipping
  if (clipped == BXR_UI_CLIP_PARTIAL) {
    _bxr_ui_command_clip(bxr_ui_clip_rect_get());
  }

  // Add command
  if (len < 0) {
    len = strlen(str);
  }

  const char *saved_str = bxr_atom_make_mem(str, len);

  cmd                  = _bxr_ui_command_push(BXR_UI_COMMAND_TEXT);
  cmd->args.text.str   = saved_str;
  cmd->args.text.pos   = pos;
  cmd->args.text.color = color;
  cmd->args.text.font  = font;

  // Reset clipping if it was set for this text
  if (clipped) {
    bxr_vec2_t frame_dimension = bxr_painter_get_frame_dimension();
    bxr_rect_t unclipped_rect  = { 0, 0, frame_dimension.x, frame_dimension.y };
    _bxr_ui_command_clip(unclipped_rect);
  }
}

// Draw an icon by pushing a new icon command.
static void
_bxr_ui_command_icon(bxr_font_t *font,
                     int icon_index,
                     bxr_rect_t rect,
                     bxr_color_t color)
{
  bxr_ui_clip_e clipped = bxr_ui_check_clip(rect);

  // If the text is fully outside the clipping rectangle, skip drawing it.
  if (clipped == BXR_UI_CLIP_OUT) {
    return;
  }

  // If the text is partially inside the clipping rectangle, set the clipping
  if (clipped == BXR_UI_CLIP_PARTIAL) {
    _bxr_ui_command_clip(bxr_ui_clip_rect_get());
  }

  _bxr_ui_command_t *cmd    = _bxr_ui_command_push(BXR_UI_COMMAND_ICON);
  cmd->args.icon.font       = _ui.state.style->font;
  cmd->args.icon.icon_index = icon_index;
  cmd->args.icon.rect       = rect;

  // Reset clipping if it was set for this text
  if (clipped) {
    bxr_vec2_t frame_dimension = bxr_painter_get_frame_dimension();
    bxr_rect_t unclipped_rect  = { 0, 0, frame_dimension.x, frame_dimension.y };
    _bxr_ui_command_clip(unclipped_rect);
  }
}

// Draw a frame (e.g. window, button) with the appropriate color and border
// based on the colorid. The border is not drawn for scrollbars and title bars.
static void
_bxr_ui_draw_frame(bxr_rect_t rect, int color_id)
{
  _bxr_ui_command_rect(rect, _ui.state.style->colors[color_id]);

  if (color_id == BXR_UI_COLOR_SCROLLBASE
      || color_id == BXR_UI_COLOR_SCROLLTHUMB
      || color_id == BXR_UI_COLOR_TITLEBG) {
    return;
  }

  // draw border
  if (_ui.state.style->colors[BXR_UI_COLOR_BORDER].a) {
    _bxr_ui_command_box(bxr_rect_expand(rect, 1),
                        _ui.state.style->colors[BXR_UI_COLOR_BORDER]);
  }
}

// Push a new layout to the layout stack with the given body and scroll offset.
static void
_bxr_ui_layout_push(bxr_rect_t body, bxr_vec2_t scroll)
{
  _bxr_ui_layout_t layout;

  int width = 0;

  memset(&layout, 0, sizeof(layout));

  layout.body
      = bxr_rect_make(body.x - scroll.x, body.y - scroll.y, body.w, body.h);
  layout.max = bxr_vec2_make(-0x1000000, -0x1000000);

  _ui.state.layouts[_ui.state.current_layout] = layout;
  _ui.state.current_layout++;

  // TODO
  // bxr_ui_layout_row(1, &width, 0);
}

// Get the current layout from the top of the layout stack.
static _bxr_ui_layout_t *
_bxr_ui_layout_get()
{
  return &_ui.state.layouts[_ui.state.current_layout - 1];
}

static void
_bxr_ui_container_bring_to_front(_bxr_ui_container_t *cnt)
{
  cnt->zindex = ++_ui.state.last_zindex;
}

static void
_bxr_ui_container_pop()
{
  // TODO
}

// Get a container by its ID or init a new one and update it if it's open.
static _bxr_ui_container_t *
_bxr_ui_container_get(Uint32 id, int opt)
{
  _bxr_ui_container_t *cnt = NULL;

  // Try to find the container in the pool using its ID.
  int idx = _bxr_ui_pool_get(
      _ui.state.container_pool, BXR_UI_CONTAINER_SLOTS_MAX, id);

  // The containe is found in the pool
  if (idx >= 0) {
    // If the container is open, update the container
    if (_ui.state.containers_data[idx].open || ~opt & BXR_UI_OPT_CLOSED) {
      _bxr_ui_pool_update(_ui.state.container_pool, idx);
    }

    // Return the container found in the pool.
    return &_ui.state.containers_data[idx];
  }

  // The container is not found in the pool and the  container is closed.
  if (opt & BXR_UI_OPT_CLOSED) {
    return NULL;
  }

  // Container not found in pool, init new container
  idx = _bxr_ui_pool_init(
      _ui.state.container_pool, BXR_UI_CONTAINER_SLOTS_MAX, id);
  cnt = &_ui.state.containers_data[idx];
  memset(cnt, 0, sizeof(*cnt));
  cnt->open = 1;

  // Bring the container to the front
  _bxr_ui_container_bring_to_front(cnt);
  return cnt;
}

// Get the current container from the top of the container stack
static _bxr_ui_container_t *
_bxr_ui_container_get_current()
{
  SDL_assert(_ui.current_state > 0);
  return _ui.state.containers[_ui.state.current_container];
}

// Get a container by its name
static _bxr_ui_container_t *
_bxr_ui_container_get_by_name(const char *name)
{
  Uint32 id = bxr_ui_get_id(name, strlen(name));
  return _bxr_ui_container_get(id, 0);
}

// Check if the current container is in the hovered root container by iterating
// through the container stack from the top to the bottom until we find the
// hovered root container or reach the current root container.
static bool
_bxr_ui_container_is_in_hovered_root()
{
  int i = _ui.state.current_container;
  while (i--) {
    if (_ui.state.containers[i] == _ui.state.hovered_root) {
      return true;
    }

    // Only root containres have thier `head` memeber set.
    // Stop searching if we've reached the current root container.
    if (_ui.state.hovered_root[i].head) {
      break;
    }
  }

  return false;
}

// Check if the mouse is over the given rectangle
static bool
_bxr_ui_is_mouse_over(bxr_rect_t rect)
{
  bxr_vec2_t mouse_pos = bxr_mouse_pos();

  // Is the mouse position within the given rectangle
  // Is the mouse position within the current clipping rectangle
  // Is the current container within the hovered root container
  return bxr_rect_contains_point(rect, mouse_pos)
         && bxr_rect_contains_point(bxr_ui_clip_rect_get(), mouse_pos)
         && _bxr_ui_container_is_in_hovered_root();
}

static void
_bxr_ui_draw_control_frame(bxr_ui_id_t id,
                           bxr_rect_t rect,
                           bxr_ui_color_e color_id,
                           bxr_ui_opt_e opt)
{
  if (opt & BXR_UI_OPT_NOFRAME) {
    return;
  }

  if (_ui.state.focused_id == id) {
    color_id = BXR_UI_COLOR_BUTTONFOCUS;
  } else if (_ui.state.hovered_id == id) {
    color_id = BXR_UI_COLOR_BUTTONHOVER;
  }

  _bxr_ui_draw_frame(rect, color_id);
}

static void
_bxr_ui_draw_control_text(const char *str,
                          bxr_rect_t rect,
                          bxr_ui_color_e color_id,
                          bxr_ui_opt_e opt)
{
  bxr_ui_clip_rect_push(rect);

  bxr_vec2_t position  = { 0, 0 };
  bxr_vec2_t text_size = bxr_text_measure(_ui.state.style->font, str);

  position.y = rect.y + (rect.h - text_size.y) * 0.5f;

  if (opt & BXR_UI_OPT_ALIGNCENTER) {
    position.x = rect.x + (rect.w - text_size.x) * 0.5f;
  } else if (opt & BXR_UI_OPT_ALIGNRIGHT) {
    position.x = rect.x + rect.w - text_size.x - _ui.state.style->padding;
  } else { // default is left align
    position.x = rect.x + _ui.state.style->padding;
  }

  bxr_ui_clip_rect_pop();
}

void
bxr_ui_update_control(bxr_ui_id_t id, bxr_rect_t rect, bxr_ui_opt_e opt)
{
  int mouse_over = _bxr_ui_is_mouse_over(rect);

  // Handle focuse update before handling no interact option.
  if (_ui.state.focused_id == id) {
    _ui.state.update_focus = true;
  }

  if (opt & BXR_UI_OPT_NOINTERACT) {
    return;
  }

  bool any_mouse_pressed = bxr_mouse_just_pressed(BXR_MOUSE_LEFT)
                           || bxr_mouse_just_pressed(BXR_MOUSE_RIGHT)
                           || bxr_mouse_just_pressed(BXR_MOUSE_MIDDLE);

  if (mouse_over && any_mouse_pressed) {
    _ui.state.hovered_id = id;
  }

  // Handle focus update
  if (_ui.state.focused_id == id) {
    // Lose focus if the mouse is just pressed outside the control
    if (any_mouse_pressed && !mouse_over) {
      _ui.state.focused_id   = 0;
      _ui.state.update_focus = true;
    }

    // Lose focus if the mouse is not pressed and the option to hold focus is
    // not enabled
    if (!any_mouse_pressed && ~opt & BXR_UI_OPT_HOLDFOCUS) {
      _ui.state.focused_id   = 0;
      _ui.state.update_focus = true;
    }
  }

  // Handle hovered update
  if (_ui.state.hovered_id == id) {
    // Set focus to the control if the mouse is just pressed on it
    if (any_mouse_pressed) {
      _ui.state.focused_id   = id;
      _ui.state.update_focus = true;
    }
    // Lose hovered if the mouse is just pressed outside the control
    else if (!mouse_over) {
      _ui.state.hovered_id = 0;
    }
  }

  // TODO update gamepad and keyboard navigation here
}

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
bxr_ui_begin(bxr_rect_t viewport)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state < BXR_UI_STATE_MAX);

  // Save current state
  _ui.states[_ui.current_state++] = _ui.state;

  _ui.state._style = default_style;
  _ui.state.style  = &_ui.state._style;

  _ui.state.viewport = viewport;

  bxr_vec2_t mouse_wheel   = bxr_mouse_wheel();
  _ui.state.scroll_delta.x = mouse_wheel.x;
  _ui.state.scroll_delta.y = mouse_wheel.y;

  bxr_vec2_t mouse_pos      = bxr_mouse_pos();
  bxr_vec2_t mouse_prev_pos = bxr_mouse_prev_pos();
  _ui.state.mouse_delta.x   = mouse_pos.x - mouse_prev_pos.x;
  _ui.state.mouse_delta.y   = mouse_pos.y - mouse_prev_pos.y;

  _ui.state.current_id      = 0;
  _ui.state.current_command = 0;

  // TODO scroll target

  // TODO hover root

  // TODO next_hover_root

  // TODO mouse_delta

  _ui.state.frame++;
}

void
bxr_ui_end(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  SDL_assert(_ui.state.current_id == 0);
  SDL_assert(_ui.state.current_container == 0);
  SDL_assert(_ui.state.current_layout == 0);
  SDL_assert(_ui.state.current_clip == 0);

  _ui.state.scroll_delta = bxr_vec2_make(0, 0);

  // TODO unset focus

  // TODO bring hover root to font

  // TODO sort containers

  // TODO set root container jumps commands

  // TODO process commands

  _ui.state = _ui.states[--_ui.current_state];
}

// 32bit fnv-1a hash
#define BXR_UI_HASH_INITIAL 2166136261

// Calculate a hash for the given data and size
static void
bxr_ui_hash(Uint32 *hash, const void *data, int size)
{
  const unsigned char *p = data;
  while (size--) {
    *hash = (*hash ^ *p++) * 16777619;
  }
}

Uint32
bxr_ui_id_get(const void *data, int size)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);

  // Get the current index of the ID stack
  int idx = _ui.state.current_id;

  // Calculate the hash based on the previous ID on the stack (or a initial
  // value if the stack is empty).
  Uint32 retval = (idx > 0) ? _ui.state.ids[idx - 1] : BXR_UI_HASH_INITIAL;

  bxr_ui_hash(&retval, data, size);

  _ui.state.last_id = retval;
  return retval;
}

void
bxr_ui_id_push(const void *data, int size)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_id < BXR_UI_ID_MAX);

  _ui.state.ids[_ui.state.current_id] = bxr_ui_get_id(data, size);
  _ui.state.current_id++;
}

void
bxr_ui_id_pop(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_id > 0);

  _ui.state.ids[_ui.state.current_id] = 0;
  _ui.state.current_id--;
}

bxr_rect_t
bxr_ui_clip_rect_get(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip > 0);

  return _ui.state.clips[_ui.state.current_clip - 1];
}

void
bxr_ui_clip_rect_push(bxr_rect_t rect)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip < BXR_UI_CLIP_MAX);

  bxr_rect_t last = bxr_ui_clip_rect_get();

  bxr_rect_t intersected = bxr_rect_get_intersection(rect, last);

  _ui.state.clips[_ui.state.current_clip] = intersected;
  _ui.state.current_clip++;
}

void
bxr_ui_clip_rect_pop(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip > 0);

  _ui.state.clips[_ui.state.current_clip] = (bxr_rect_t){ 0 };
  _ui.state.current_clip--;
}

bxr_ui_clip_e
bxr_ui_clip_check(bxr_rect_t rect)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  bxr_rect_t clip_rect = bxr_ui_clip_rect_get();

  if (bxr_rect_overlap(rect, clip_rect)) {
    if (bxr_rect_contains(clip_rect, rect)) {
      return BXR_UI_CLIP_IN;
    } else {
      return BXR_UI_CLIP_PARTIAL;
    }
  } else {
    return BXR_UI_CLIP_OUT;
  }
}

void
bxr_ui_layout_begin_column(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_layout < BXR_UI_STATE_MAX);

  _bxr_ui_layout_push(bxr_ui_layout_next(), bxr_vec2_make(0, 0));
}

void
bxr_ui_layout_end_column(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_layout > 0);

  _bxr_ui_layout_t *parent, *current;

  current = _bxr_ui_layout_get();

  // Pop the current layout from the stack (don't overwrite it, we need it to
  // update the parent layout with the dimensions of the current layout).
  _ui.state.current_layout--;

  parent = _bxr_ui_layout_get();

  // Update the parent layout cursor position based on the current layout
  // dimensions and position.

  parent->cursor.x = bxr_max(
      parent->cursor.x, current->cursor.x + current->body.x - parent->body.x);

  parent->cursor.y
      = bxr_max(parent->next_row_y,
                current->next_row_y + parent->body.y - current->body.y);

  parent->max.x = bxr_max(parent->max.x, current->max.x);
  parent->max.y = bxr_max(parent->max.y, current->max.y);
}

void
bxr_ui_layout_width(int width)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  _bxr_ui_layout_t *layout = _bxr_ui_layout_get();
  layout->dimensions.x     = width;
}

void
bxr_ui_layout_height(int height)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  _bxr_ui_layout_t *layout = _bxr_ui_layout_get();
  layout->dimensions.y     = height;
}

void
bxr_ui_layout_set_next(bxr_rect_t rect, bool relative)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  _bxr_ui_layout_t *layout = _bxr_ui_layout_get();

  layout->next = rect;
  layout->next_type
      = relative ? BXR_UI_LAYOUT_RELATIVE : BXR_UI_LAYOUT_ABSOLUTE;
}

bxr_rect_t
bxr_ui_layout_next(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);

  _bxr_ui_layout_t *layout = _bxr_ui_layout_get();
  _bxr_ui_style_t *style   = _ui.state.style;

  bxr_rect_t retval = { 0 };

  if (layout->next_type) {            // Relative or Absolute layout
    layout->next_type = 0;            // Reset next type for the next element
    retval            = layout->next; // Get the next layout rect

    _bxr_ui_layout_e type = layout->next_type;
    if (type == BXR_UI_LAYOUT_ABSOLUTE) {
      return retval;
    }
  } else { // No type is set
    retval.x = layout->cursor.x;
    retval.y = layout->cursor.y;

    // Set the width of the layout column
    retval.w = layout->items > 0 ? layout->widths[layout->item_index]
                                 : style->layout_dimensions.x;
    // Set the height of the layout row
    retval.h = style->layout_dimensions.y;

    // Ensure that dimensions are not zero or negative, if they are set them
    // to the default size
    if (retval.w == 0) {
      retval.w = style->layout_dimensions.x + style->padding * 2;
    }
    if (retval.h == 0) {
      retval.h = style->layout_dimensions.y + style->padding * 2;
    }
    if (retval.w < 0) {
      retval.w = layout->body.w - retval.x + 1;
    }
    if (retval.h < 0) {
      retval.h = layout->body.h - retval.y + 1;
    }

    layout->item_index++;
  }

  // Update cursor position for the next element (only for x)
  layout->cursor.x += retval.w + style->spacing;

  // Update the next row y position (not the cursor y, this is done at the end
  // of the current layout)
  layout->next_row_y
      = bxr_max(layout->next_row_y, retval.y + retval.h + style->spacing);

  // Apply the body offset to the layout rect
  retval.x += layout->body.x;
  retval.y += layout->body.y;

  // Update max position for the layout
  layout->max.x = bxr_max(layout->max.x, retval.x + retval.w);
  layout->max.y = bxr_max(layout->max.y, retval.y + retval.h);

  return retval;
}
