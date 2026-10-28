#include <boxer/bxr.h>

#include "player.h"

void
player_update(player_t *player, animation_t *animation, bxr_vec2_t *velocity)
{
  // TODO: input -> state transitions; on state change:
  // animation_set_clip(animation, player->state);
  animation->flip_x = (player->direction == DIR_LEFT);
}
