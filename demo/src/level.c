#include "level.h"
#include "defs.h"
#include "game.h"
#include "map.h"
#include "scene.h"

scene_t SCENE_LEVEL = {
  .setup    = level_setup,
  .update   = level_update,
  .render   = level_render,
  .teardown = level_teardown,
};

static map_t *_map   = NULL;
static game_t *_game = NULL;

static bxr_audio_t _music;
static bxr_voice_t _music_voice;

void
level_setup(void)
{
  _map = map_create(ASSET_LEVEL_TILESET,
                    LEVEL_MAP,
                    LEVEL_TILE_SIZE,
                    LEVEL_MAP_WIDTH,
                    LEVEL_MAP_HEIGHT);

  if (!_map) {
    BXR_LOG_ERROR("Failed to load map");
    return;
  }

  _game = game_create(_map, LEVEL_SPAWN_POSITION);
  if (!_game) {
    BXR_LOG_ERROR("Failed to create game");
    return;
  }

  _music = bxr_audio_create(ASSET_LEVEL_MUSIC);

  bxr_voice_desc_t music_desc = {
    .audio      = _music,
    .volume     = 1.0f,
    .auto_play  = true,
    .play_once  = false,
    .keep_alive = true,
  };

  _music_voice = bxr_voice_create(&music_desc);
}

void
level_update(double delta_ms)
{
  game_update(_game, delta_ms);
}

void
level_render(double alpha_ms)
{
  game_render(_game, alpha_ms);
}

void
level_teardown(void)
{
  bxr_voice_destroy(_music_voice);
  bxr_audio_destroy(_music);

  game_destroy(_game);
  map_destroy(_map);
}
