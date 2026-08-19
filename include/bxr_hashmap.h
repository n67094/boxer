/**
 * File `bxr_hashmap_t.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * A Data Oriented Design (DOD) hashmap implementation in C This hash map is
 * designed to map keys to a list of values/buckets.
 */

#ifndef BXR_HASHMAP_H_
#define BXR_HASHMAP_H_

#include <SDL3/SDL.h>

#define BXR_HASHMAP_MIN_SIZE 16

// Load factor is 0.75 (3/4) a good tune for performance and memory usage.
#define BXR_HASHMAP_LOAD_NUMERATOR 3
#define BXR_HASHMAP_LOAD_DENOMINATOR 4

typedef enum
{
  BXR_HASHMAP_SLOT_EMPTY     = 0, // Empty slot
  BXR_HASHMAP_SLOT_OCCUPIED  = 1, // Occupied slot
  BXR_HASHMAP_SLOT_TOMBSTONE = 2  // Deleted slot (tombstone)
} bxr_hashmap_slot_e;

typedef struct hashmap_bucket_s
{
  Uint32 *ids; // Resource ids
  size_t count;
  size_t size; // Power of two, or 0 if unallocated
} hashmap_bucket_t;

typedef struct bxr_hashmap_s bxr_hashmap_t;

bxr_hashmap_t *bxr_hashmap_create(size_t initial_size);

bool bxr_hashmap_push(bxr_hashmap_t *map, Uint32 key, Uint32 value);

bool bxr_hashmap_remove(bxr_hashmap_t *map, Uint32 key, Uint32 value);

bool bxr_hashmap_remove_bucket(bxr_hashmap_t *map, Uint32 key);

bool bxr_hashmap_get_bucket(bxr_hashmap_t *map,
                            Uint32 key,
                            hashmap_bucket_t *out_values);

void bxr_hashmap_destroy(bxr_hashmap_t *map);

#endif // BXR_HASHMAP_H_
