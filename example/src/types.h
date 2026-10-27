/**
 * @file types.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TYPES_H_
#define TYPES_H_

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

typedef struct animation_s
{
  state_e state;
  int from;
  int to;
  Uint64 frame_ms;   // Duration of a frame in milliseconds
  Uint64 elapsed_ms; // Time elapsed since the last frame change
} animation_t;

typedef struct player_s
{
  state_e state;
  direction_e direction;
  bool on_ground;
  float speed;
} player_t;

#endif // TYPES_H_
