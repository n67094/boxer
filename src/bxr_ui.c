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
  bxr_ui_style_t _style;
  bxr_ui_style_t *style; // style pointer for modification

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

  // Clip stack
  Uint32 current_clip;
  bxr_rect_t clips[BXR_UI_CLIP_MAX];

  // Layout stack
  Uint32 current_layout;
  _bxr_ui_layout_t layouts[BXR_UI_STATE_MAX];

  // Containers data
  _bxr_ui_container_t containers_data[BXR_UI_CONTAINER_SLOTS_MAX];

  // TODO pool

  // TODO missing members

  int frame;
} _bxr_ui_state_t;

typedef struct _bxr_ui_s
{
  // States stack
  Uint32 current_state;
  _bxr_ui_state_t states[BXR_UI_STATE_MAX];
  _bxr_ui_state_t state; // current state
} _bxr_ui_t;

typedef struct _bxr_ui_pool_item_s
{
  Uint32 id;
  int last_update;
} _bxr_ui_pool_item_t;

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

  // TODO handle inputs

  // Save current state
  _ui.states[_ui.current_state++] = _ui.state;

  _ui.state._style = default_style;
  _ui.state.style  = &_ui.state._style;

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

  // TODO Check stack
  // TODO handle scroll input
  // TODO unset focus
  // TODO bring hover root to font
  // TODO Reset input state

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

// Get a container by its ID and update it if it's open or the `MU_OPT_CLOSED`
// flag is not set. If the container is not found in the pool and the
// `MU_OPT_CLOSED` flag is set, return NULL. Otherwise, initialize a new
// container and return it.
static _bxr_ui_container_t *
_bxr_ui_get_container(Uint32 id, int opt)
{
  _bxr_ui_container_t *cnt = NULL;

  /*
  // Try to find the container in the pool using its ID.
  int idx = mu_pool_get(ctx, ctx->container_pool, MU_CONTAINERPOOL_SIZE, id);

  // The containe is found in the pool
  if (idx >= 0) {
    // If the container is open or the `MU_OPT_CLOSED` flag is not set, update
    // the container
    if (ctx->containers[idx].open || ~opt & MU_OPT_CLOSED) {
      mu_pool_update(ctx, ctx->container_pool, idx);
    }

    // Return the container found in the pool.
    return &ctx->containers[idx];
  }

  // The container is not found in the pool and the `MU_OPT_CLOSED` flag is set,
  // return NULL.
  if (opt & MU_OPT_CLOSED) {
    return NULL;
  }

  // Container not found in pool: init new container
  idx = mu_pool_init(ctx, ctx->container_pool, MU_CONTAINERPOOL_SIZE, id);
  cnt = &ctx->containers[idx];
  memset(cnt, 0, sizeof(*cnt));
  cnt->open = 1;
  mu_bring_to_front(ctx, cnt);
  */

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

static void
_bxr_ui_bring_to_front(_bxr_ui_container_t *cnt)
{
  cnt->zindex = ++_ui.state.last_zindex;
}
