#include <boxer/bxr.h>

#include "map.h"

map_t *
map_create(const char *tileset_path,
           const Uint32 *tiles_data,
           Uint32 tile_size,
           Uint32 width,
           Uint32 height)
{
  BXR_ASSERT(tileset_path != NULL && "tileset_path must not be NULL");
  BXR_ASSERT(tiles_data != NULL && "tiles_data must not be NULL");

  map_t *map = NULL;
  BXR_NEW(map);
  if (!map) {
    BXR_LOG_ERROR("Failed to create map");
    return NULL;
  }

  map->tileset   = bxr_image_create(tileset_path);
  map->tiles     = BXR_ALLOC(sizeof(Uint32) * width * height);
  map->tile_size = tile_size;
  map->width     = width;
  map->height    = height;

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

  int start_x = (viewport.x / map->tile_size);
  int start_y = (viewport.y / map->tile_size);
  int end_x   = ((viewport.x + viewport.w) / map->tile_size) + 1;
  int end_y   = ((viewport.y + viewport.h) / map->tile_size) + 1;

  // Clamp to the map, so no bounds check is needed inside the loop
  if (start_x < 0)
    start_x = 0;
  if (start_y < 0)
    start_y = 0;
  if (end_x > (int)map->width)
    end_x = map->width;
  if (end_y > (int)map->height)
    end_y = map->height;

  int tileset_w = bxr_image_get_width(map->tileset);
  int columns   = tileset_w / map->tile_size;

  for (int y = start_y; y < end_y; ++y) {
    for (int x = start_x; x < end_x; ++x) {
      // Get the tile index from the map's tile data
      Uint32 tile_index = map->tiles[y * map->width + x];

      bxr_rect_t src_rect = {
        .x = (tile_index % columns) * map->tile_size,
        .y = (tile_index / columns) * map->tile_size,
        .w = map->tile_size,
        .h = map->tile_size,
      };

      bxr_rect_t dst_rect = {
        .x = x * (float)map->tile_size - viewport.x,
        .y = y * (float)map->tile_size - viewport.y,
        .w = map->tile_size,
        .h = map->tile_size,
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
  Uint32 tile_x = (Uint32)(position.x / map->tile_size);
  Uint32 tile_y = (Uint32)(position.y / map->tile_size);

  // Check if the tile coordinates are within the map bounds
  if (tile_x >= map->width || tile_y >= map->height) {
    return false;
  }

  // Get the tile index from the map's tile data
  Uint32 tile_index = map->tiles[tile_y * map->width + tile_x];

  return tile_index >= 1;
}

bxr_rect_t
map_get_bounds(map_t *map)
{
  BXR_ASSERT(map != NULL && "map must not be NULL");

  bxr_rect_t bounds = {
    .x = 0,
    .y = 0,
    .w = map->width * map->tile_size,
    .h = map->height * map->tile_size,
  };

  return bounds;
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
