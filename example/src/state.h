/**
 * @file state.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef STATE_H_
#define STATE_H_

typedef enum
{
  STATE_MENU = 0,
  STATE_LEVEL,
} game_state_e;

typedef struct state_s state_t;

void state_setup(void);
void state_teardown(void);

void state_set(game_state_e new_state);
game_state_e state_get(void);

#endif // STATE_H_
