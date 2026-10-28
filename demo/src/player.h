/**
 * @file player.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <boxer/bxr.h>

#include "animation.h"
#include "defs.h"

// Math: h = v²/2g
#define PLAYER_GRAVITY 0.0006f   // g
#define PLAYER_JUMP_SPEED 0.286f // v
// h = v²/2g = 0.286²/(2*0.0006) = ~68 pixels, 8px per tiles = ~8 tiles

#define PLAYER_SPEED 0.1f
#define PLAYER_MAX_FALL 0.4f // < 1 tile per tick, keeps collision simple
#define PLAYER_JUMP_CUT 0.5f // velocity kept when A is released early

typedef enum
{
  PLAYER_STATE_IDLE = 0,
  PLAYER_STATE_WALKING,
  PLAYER_STATE_DUCKING,
  PLAYER_STATE_RAISING,
  PLAYER_STATE_FALLING,
  PLAYER_STATE_SIZE,
} state_e;

typedef enum
{
  DIR_LEFT = 0,
  DIR_RIGHT,
} direction_e;

typedef struct player_s
{
  state_e state;
  direction_e direction;
  bool on_ground;
  float speed;
} player_t;

static const clip_t PLAYER_CLIPS[PLAYER_STATE_SIZE] = {
  [PLAYER_STATE_IDLE]    = { .from = 0, .to = 0, .frame_ms = 150 },
  [PLAYER_STATE_WALKING] = { .from = 1, .to = 4, .frame_ms = 150 },
  [PLAYER_STATE_DUCKING] = { .from = 5, .to = 5, .frame_ms = 150 },
  [PLAYER_STATE_RAISING] = { .from = 6, .to = 6, .frame_ms = 150 },
  [PLAYER_STATE_FALLING] = { .from = 7, .to = 7, .frame_ms = 150 },
};

static const player_t PLAYER_DEFAULT = {
  .state     = PLAYER_STATE_IDLE,
  .direction = DIR_RIGHT,
  .on_ground = true,
  .speed     = PLAYER_SPEED,
};

static const bxr_rect_t PLAYER_COLLIDER = { .x = 0, .y = 4, .w = 8, .h = 12 };

static const animation_desc_t PLAYER_ANIMATION_DESC = {
  .spritesheet_path = ASSET_PLAYER_SPRITESHEET,
  .frame_width      = 8,
  .frame_height     = 16,
  .clips            = PLAYER_CLIPS,
  .clip_count       = PLAYER_STATE_SIZE,
};

/**
 * Updates the player's state based on input and physics.
 *
 * `player` is a pointer to the player instance to update.
 *
 * `animation` is a pointer to the animation instance associated with the
 * player.
 *
 * `velocity` is a pointer to the player's velocity vector, which will be
 * modified based on input and physics.
 */
void
player_update(player_t *player, animation_t *animation, bxr_vec2_t *velocity);

#endif // PLAYER_H_
