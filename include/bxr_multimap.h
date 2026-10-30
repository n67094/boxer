/**
 * File `bxr_multimap.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # String Multimap
 *
 * Hash map from string keys to sets of `Uint32` values (boxer resource ids).
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
 * Create an empty multimap.
 *
 * `initial_size` initial slot count. Rounded up to the next power of two, with
 * a minimum of `BXR_MULTIMAP_MIN_SIZE`; 0 is allowed.
 *
 * `return` the new multimap, or NULL on allocation failure.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller must free the result with `bxr_multimap_destroy()`.
 */
bxr_multimap_t *bxr_multimap_create(size_t initial_size);

/**
 * Add a value to the set of values of a key.
 *
 * The key is created if missing. Adding a value already present for the key
 * is a no-op that succeeds.
 *
 * `map` the multimap; must not be NULL.
 * `key` the key; must not be NULL. The string is copied.
 * `value` the value to add.
 *
 * `return` true on success, false on allocation failure.
 * Call `bxr_get_error()` for more information.
 */
bool bxr_multimap_add(bxr_multimap_t *map, const char *key, Uint32 value);

/**
 * Remove a value from the set of values of a key.
 *
 * Value order in the bucket is not preserved. The key is removed when its last
 * value is removed.
 *
 * `map` the multimap; must not be NULL.
 * `key` the key; must not be NULL.
 * `value` the value to remove.
 *
 * `return` true if the value was removed, false if the key or the value was
 * not found.
 */
bool
bxr_multimap_remove_value(bxr_multimap_t *map, const char *key, Uint32 value);

/**
 * Remove a key and all its values.
 *
 * `map` the multimap; must not be NULL.
 * `key` the key; must not be NULL.
 *
 * `return` true if the key was removed, false if it was not found.
 */
bool bxr_multimap_remove(bxr_multimap_t *map, const char *key);

/**
 * Get a copy of the values of a key, in no particular order.
 *
 * `map` the multimap; must not be NULL.
 * `key` the key; must not be NULL.
 * `out_values` receives a newly allocated array of the values, or NULL if the
 * key does not exist. May be NULL to only query the count.
 *
 * `return` the number of values, or 0 if the key does not exist or the
 * allocation fails.
 *
 * **Note**: the caller must free `*out_values` with `BXR_FREE()`.
 */
size_t bxr_multimap_get_values(bxr_multimap_t *map,
                               const char *key,
                               Uint32 **out_values);

/**
 * Get the number of keys.
 *
 * `map` the multimap; must not be NULL.
 *
 * `return` the number of keys, not the total number of values.
 */
size_t bxr_multimap_count(bxr_multimap_t *map);

/**
 * Destroy a multimap and free all its keys and values.
 *
 * `map` the multimap to destroy.
 */
void bxr_multimap_destroy(bxr_multimap_t *map);

#endif // BXR_MULTIMAP_H_
