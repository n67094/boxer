#include <boxer/bxr.h>

#include "input.h"
#include "player.h"

static state_e
player_resolve_state(const player_t *player, const bxr_vec2_t *velocity)
{
  if (player->on_ground) {
    return velocity->x != 0.0f ? PLAYER_STATE_WALKING : PLAYER_STATE_IDLE;
  }

  if (velocity->y < -PLAYER_JUMP_SPEED * 0.5f) {
    return PLAYER_STATE_DUCKING;
  }

  return velocity->y < 0.0f ? PLAYER_STATE_RAISING : PLAYER_STATE_FALLING;
}

void
player_update(player_t *player, animation_t *animation, bxr_vec2_t *velocity)
{
  // Horizontal movement
  velocity->x = 0.0f;
  if (input_held(INPUT_BUTTON_LEFT)) {
    velocity->x -= player->speed;
    player->direction = DIR_LEFT;
  }
  if (input_held(INPUT_BUTTON_RIGHT)) {
    velocity->x += player->speed;
    player->direction = DIR_RIGHT;
  }

  // Jump
  if (player->on_ground && input_just_pressed(INPUT_BUTTON_A)) {
    velocity->y       = -PLAYER_JUMP_SPEED;
    player->on_ground = false;
  }

  // Jump cut (release A early)
  if (!player->on_ground && velocity->y < 0.0f && !input_held(INPUT_BUTTON_A)) {
    velocity->y *= PLAYER_JUMP_CUT;
  }

  state_e state = player_resolve_state(player, velocity);
  if (state != player->state) {
    player->state = state;
    animation_set_clip(animation, player->state);
  }

  animation->flip_x = (player->direction == DIR_LEFT);
}
