/**
 * File `bxr_multimap_t.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * A Data Oriented Design (DOD) multimap implementation in C This hash map is
 * designed to map keys to set of values.
 */

#ifndef BXR_MULTIMAP_H_
#define BXR_MULTIMAP_H_

#include <SDL3/SDL.h>

#define BXR_MULTIMAP_MIN_SIZE 16

// Load factor is 0.75 (3/4) a good tune for performance and memory usage.
#define BXR_MULTIMAP_LOAD_NUMERATOR 3
#define BXR_MULTIMAP_LOAD_DENOMINATOR 4

typedef enum
{
  BXR_MULTIMAP_SLOT_EMPTY     = 0, // Empty slot
  BXR_MULTIMAP_SLOT_OCCUPIED  = 1, // Occupied slot
  BXR_MULTIMAP_SLOT_TOMBSTONE = 2  // Deleted slot (tombstone)
} bxr_multimap_slot_e;

typedef struct bxr_multimap_s bxr_multimap_t;

/**
 * Create a new multimap.
 *
 * `initial_size` The initial size of the multimap. Must be a power of two. If
 * not it will be rouned up to the next power of two.
 *
 * `return` A pointer to the newly created multimap, or NULL on failure.
 */
bxr_multimap_t *bxr_multimap_create(size_t initial_size);

/**
 * Push a value to the multimap.
 *
 * `map` The multimap to push the value to. Cannot be NULL.
 *
 * `key` The key to push the value to.
 *
 * `value` The value to push to the multimap.
 *
 * `return` true on success, false on failure.
 */
bool bxr_multimap_push(bxr_multimap_t *map, const char *key, Uint32 value);

/**
 * Remove a value from the multimap. All duplicates of the value will be removed
 * from the bucket.
 *
 * `map` The multimap to remove the value from. Cannot be NULL.
 *
 * `key` The key to remove the value from.
 *
 * `value` The value to remove from the multimap.
 *
 * `return` true on success, false on failure.
 */
bool
bxr_multimap_remove_value(bxr_multimap_t *map, const char *key, Uint32 value);

/**
 * Remove the key and all its values from the multimap.
 *
 * `map` The multimap to remove the bucket from. Cannot be NULL.
 *
 * `key` The key to remove the bucket from.
 *
 * `return` true on success, false on failure.
 */
bool bxr_multimap_remove(bxr_multimap_t *map, const char *key);

/**
 * Get the values from a bucket in the multimap.
 *
 * `map` The multimap to get the bucket from. Cannot be NULL.
 *
 * `key` The key to get the bucket from.
 *
 * `out_values` A pointer a copy of the values in the bucket. The caller is
 * responsible for freeing the memory allocated for the values.
 *
 * `return` the number of values in the bucket, or 0 if the bucket does not
 * exist.
 */
size_t bxr_multimap_get_values(bxr_multimap_t *map,
                               const char *key,
                               Uint32 **out_values);

size_t bxr_multimap_count(bxr_multimap_t *map);

/**
 * Destroy a multimap.
 *
 * `map` The multimap to destroy. Cannot be NULL.
 */
void bxr_multimap_destroy(bxr_multimap_t *map);

#endif // BXR_MULTIMAP_H_
