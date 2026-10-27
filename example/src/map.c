#include <boxer/bxr.h>

#include "map.h"

map_t *
map_create(const char *tileset_path,
           const Uint32 *tiles_data,
           Uint32 width,
           Uint32 height)
{
  BXR_ASSERT(tileset_path != NULL && "tileset_path must not be NULL");
  BXR_ASSERT(tiles_data != NULL && "tiles_data must not be NULL");
  BXR_ASSERT(width > 0 && "width must be greater than 0");
  BXR_ASSERT(height > 0 && "height must be greater than 0");

  map_t *map = NULL;
  BXR_NEW(map);
  if (!map) {
    BXR_LOG_ERROR("Failed to create map");
    return NULL;
  }

  map->tileset = bxr_image_create(tileset_path);
  map->tiles   = BXR_ALLOC(sizeof(Uint32) * width * height);

  BXR_MEMCPY(map->tiles, tiles_data, sizeof(Uint32) * width * height);

  return map;
}

void
map_render(map_t *map, bxr_rect_t viewport)
{
  BXR_ASSERT(map != NULL && "map must not be NULL");

  bxr_painter_set_blendmode(BXR_BLENDMODE_BLEND);
  bxr_painter_set_color(BXR_COLOR_WHITE);
  bxr_painter_set_image(0, map->tileset);

  Uint32 start_x = (Uint32)(viewport.x / MAP_TILE_SIZE);
  Uint32 start_y = (Uint32)(viewport.y / MAP_TILE_SIZE);
  Uint32 end_x   = (Uint32)((viewport.x + viewport.w) / MAP_TILE_SIZE) + 1;
  Uint32 end_y   = (Uint32)((viewport.y + viewport.h) / MAP_TILE_SIZE) + 1;

  for (Uint32 y = start_y; y < end_y; ++y) {
    for (Uint32 x = start_x; x < end_x; ++x) {
      // Skip tiles that are outside the map bounds
      if (x >= map->width || y >= map->height) {
        continue;
      }

      // Get the tile index from the map's tile data
      Uint32 tile_index = map->tiles[y * map->width + x];

      int tileset_w = bxr_image_get_width(map->tileset);

      int tile_x = (tile_index % (tileset_w / MAP_TILE_SIZE)) * MAP_TILE_SIZE;
      int tile_y = (tile_index / (tileset_w / MAP_TILE_SIZE)) * MAP_TILE_SIZE;

      bxr_rect_t src_rect = {
        .x = tile_x,
        .y = tile_y,
        .w = MAP_TILE_SIZE,
        .h = MAP_TILE_SIZE,
      };

      bxr_rect_t dst_rect = {
        .x = x * MAP_TILE_SIZE,
        .y = y * MAP_TILE_SIZE,
        .w = MAP_TILE_SIZE,
        .h = MAP_TILE_SIZE,
      };

      bxr_painter_draw_textured_rect(
          0, (bxr_textured_rect_t){ .dst = dst_rect, .src = src_rect });
    }
  }

  bxr_painter_reset_image(0);
  bxr_painter_set_blendmode(BXR_BLENDMODE_NONE);
}

bool
map_is_solid(map_t *map, bxr_vec2_t position)
{
  BXR_ASSERT(map != NULL && "map must not be NULL");

  // Convert the position to tile coordinates
  Uint32 tile_x = (Uint32)(position.x / MAP_TILE_SIZE);
  Uint32 tile_y = (Uint32)(position.y / MAP_TILE_SIZE);

  // Check if the tile coordinates are within the map bounds
  if (tile_x >= map->width || tile_y >= map->height) {
    return false;
  }

  // Get the tile index from the map's tile data
  Uint32 tile_index = map->tiles[tile_y * map->width + tile_x];

  return tile_index >= 1;
}

void
map_destroy(map_t *map)
{
  if (!map) {
    return;
  }

  BXR_FREE(map->tiles);
  bxr_image_destroy(map->tileset);
  BXR_FREE(map);
}
