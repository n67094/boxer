/**
 * @file map.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef MAP_H_
#define MAP_H_

#include <boxer/bxr.h>

typedef struct map_s
{
  bxr_image_t tileset;
  Uint32 *tiles;
  Uint32 tile_size;
  Uint32 width;
  Uint32 height;
} map_t;

/**
 * Creates a new map with the given tileset and tile data.
 *
 * `tileset_path` is the path to the tileset image file.
 *
 * `tiles_data` is an array of tile indices that define the map layout.
 *
 * `width` and `height` are the dimensions of the map in tiles.
 *
 * `return` a pointer to the newly created map, or NULL otherwise.
 */
map_t *map_create(const char *tileset_path,
                  const Uint32 *tiles_data,
                  Uint32 tile_size,
                  Uint32 width,
                  Uint32 height);

/**
 * Renders the given map to the screen using the provided viewport.
 *
 * `map` is a pointer to the map to render.
 *
 * `viewport` is the rectangle that defines the area of the map to render.
 *
 * **NOTE:** Must be called within the game render loop.
 */
void map_render(map_t *map, bxr_rect_t viewport);

/**
 * Checks if the tile at the given position in the map is ground.
 *
 * `map` is a pointer to the map to check.
 *
 * `rect` is the position in world space to check for solidity.
 *
 * `return` true if the tile at the given position is solid, false otherwise.
 */
bool map_is_solid(map_t *map, bxr_vec2_t position);

/**
 * Checks if the given rectangle in the map is solid.
 *
 * `map` is a pointer to the map to check.
 *
 * `rect` is the rectangle in world space to check for solidity.
 *
 * `return` true if any part of the rectangle overlaps with solid tiles, false
 * otherwise.
 */
bool map_rect_is_solid(map_t *map, bxr_rect_t rect);

/**
 * Gets the bounds of the given map in world space.
 *
 * `map` is a pointer to the map to get the bounds of.
 *
 * `return` a bxr_rect_t that describes the bounds of the map in world space.
 */
bxr_rect_t map_get_bounds(map_t *map);

/**
 * Destroys the given map and frees its resources.
 *
 * `map` is a pointer to the map to destroy.
 */
void map_destroy(map_t *map);

#endif // MAP_H_
