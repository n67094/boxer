#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_font.h"
#include "bxr_keyboard.h"
#include "bxr_math.h"
#include "bxr_mouse.h"
#include "bxr_painter.h"
#include "bxr_text.h"
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
  bxr_rect_t pos;
  bxr_color_t color;
  char str[1];
} _bxr_ui_text_command_t;

typedef union _bxr_ui_command_args_u
{
  _bxr_ui_base_command_t base;
  _bxr_ui_jump_command_t jump;
  _bxr_ui_clip_command_t clip;
  _bxr_ui_rect_command_t rect;
  _bxr_ui_text_command_t text;

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
  bxr_ui_style_t _style;
  bxr_ui_style_t *style; // style pointer for modification

  bxr_rect_t viewport;

  Uint32 last_id; // last generated ID
  int last_zindex;

  // TODO missing memeber

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

// Push a new layout to the layout stack with the given body and scroll offset.
static void
_bxr_ui_push_layout(bxr_rect_t body, bxr_vec2_t scroll)
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
_bxr_ui_get_layout()
{
  return &_ui.state.layouts[_ui.state.current_layout - 1];
}

static void
_bxr_ui_bring_to_front(_bxr_ui_container_t *cnt)
{
  cnt->zindex = ++_ui.state.last_zindex;
}

// Get a container by its ID or init a new one and update it if it's open.
static _bxr_ui_container_t *
_bxr_ui_get_container(Uint32 id, int opt)
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
  _bxr_ui_bring_to_front(cnt);
  return cnt;
}

// Get the current container from the top of the container stack
static _bxr_ui_container_t *
_bxr_ui_get_current_container()
{
  SDL_assert(_ui.current_state > 0);
  return _ui.state.containers[_ui.state.current_container];
}

// Get a container by its name
static _bxr_ui_container_t *
_bxr_ui_get_container_by_name(const char *name)
{
  Uint32 id = bxr_ui_get_id(name, strlen(name));
  return _bxr_ui_get_container(id, 0);
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
bxr_ui_begin()
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state < BXR_UI_STATE_MAX);

  // Save current state
  _ui.states[_ui.current_state++] = _ui.state;

  _ui.state._style = default_style;
  _ui.state.style  = &_ui.state._style;

  // TODO get the viewport bounds from the painter.
  // This will be used to calculate mouse position relative to the viewport.
  // _ui.state.viewport = viewport;

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

/* 32bit fnv-1a hash */
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
bxr_ui_get_id(const void *data, int size)
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
bxr_ui_push_id(const void *data, int size)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_id < BXR_UI_ID_MAX);

  _ui.state.ids[_ui.state.current_id] = bxr_ui_get_id(data, size);
  _ui.state.current_id++;
}

void
bxr_ui_pop_id(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_id > 0);

  _ui.state.ids[_ui.state.current_id] = 0;
  _ui.state.current_id--;
}

void
bxr_ui_push_clip_rect(bxr_rect_t rect)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip < BXR_UI_CLIP_MAX);

  bxr_rect_t last = bxr_ui_get_clip_rect();

  bxr_rect_t intersected = bxr_rect_get_intersection(rect, last);

  _ui.state.clips[_ui.state.current_clip] = intersected;
  _ui.state.current_clip++;
}

void
bxr_ui_pop_clip_rect(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip > 0);

  _ui.state.clips[_ui.state.current_clip] = (bxr_rect_t){ 0 };
  _ui.state.current_clip--;
}

bxr_rect_t
bxr_ui_get_clip_rect(void)
{
  SDL_assert(_initialized == BXR_INIT_COOKIE);
  SDL_assert(_ui.current_state > 0);
  SDL_assert(_ui.state.current_clip > 0);

  return _ui.state.clips[_ui.state.current_clip - 1];
}

bxr_ui_clip_e
bxr_ui_check_clip(bxr_rect_t rect)
{
  bxr_rect_t clip_rect = bxr_ui_get_clip_rect();

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
_bxr_ui_command_push_jump(_bxr_ui_command_t *dst)
{
  _bxr_ui_command_t *cmd = _bxr_ui_command_push(BXR_UI_COMMAND_JUMP);
  cmd->args.jump.dst     = dst;
  return cmd;
}

// Set the current clipping rectangle by pushing a new clip command.
static void
_bxr_ui_set_clip(bxr_rect_t rect)
{
  _bxr_ui_command_t *cmd;
  cmd                 = _bxr_ui_command_push(BXR_UI_COMMAND_CLIP);
  cmd->args.clip.rect = rect;
}

// Draw a filled rectangle by pushing a new rect command.
static void
_bxr_ui_draw_rect(bxr_rect_t rect, bxr_color_t color)
{
  _bxr_ui_command_t *cmd;

  rect = bxr_rect_get_intersection(rect, bxr_ui_get_clip_rect());

  if (rect.w > 0 && rect.h > 0) {
    cmd                  = _bxr_ui_command_push(BXR_UI_COMMAND_RECT);
    cmd->args.rect.rect  = rect;
    cmd->args.rect.color = color;
  }
}

// Draw a border by drawing four rectangles around the edges of the given
// rectangle.
static void
_bxr_ui_draw_box(bxr_rect_t rect, bxr_color_t color)
{
  _bxr_ui_draw_rect(bxr_rect_make(rect.x + 1, rect.y, rect.w - 2, 1), color);
  _bxr_ui_draw_rect(
      bxr_rect_make(rect.x + 1, rect.y + rect.h - 1, rect.w - 2, 1), color);
  _bxr_ui_draw_rect(bxr_rect_make(rect.x, rect.y, 1, rect.h), color);
  _bxr_ui_draw_rect(bxr_rect_make(rect.x + rect.w - 1, rect.y, 1, rect.h),
                    color);
}

// Draw text by pushing a new text command.
static void
_bxr_ui_draw_text(bxr_font_t *font,
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
    _bxr_ui_set_clip(bxr_ui_get_clip_rect());
  }

  // Add command
  if (len < 0) {
    len = strlen(str);
  }

  // TODO add command with text (the text should be a pointer to a string
  // stash).

  /*
  cmd = mu_push_command(ctx, MU_COMMAND_TEXT, sizeof(mu_TextCommand) + len);
  memcpy(cmd->text.str, str, len);
  cmd->text.str[len] = '\0';
  cmd->text.pos      = pos;
  cmd->text.color    = color;
  cmd->text.font     = font;
  */

  // Reset clipping if it was set for this text
  if (clipped) {
    bxr_vec2_t frame_dimension = bxr_painter_get_frame_dimension();
    bxr_rect_t unclipped_rect  = { 0, 0, frame_dimension.x, frame_dimension.y };
    _bxr_ui_set_clip(unclipped_rect);
  }
}
