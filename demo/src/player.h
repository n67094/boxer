/**
 * @file player.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <boxer/bxr.h>

typedef enum
{
  STATE_IDLE = 0,
  STATE_WALKING,
  STATE_JUMPING,
  STATE_FALLING,
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

#endif // PLAYER_H_
