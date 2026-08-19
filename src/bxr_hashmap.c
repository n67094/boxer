#include "bxr_hashmap.h"
#include "bxr_assert.h"
#include "bxr_mem.h"

struct bxr_hashmap_s
{
  bxr_hashmap_slot_e *state;
  Uint64 *hashes; // Avoids rehashing keys
  Uint64 *keys;
  hashmap_bucket_t *values; // Map keys to a bucket of ids
  size_t count;             // Count of occupied slots (excluding tombstones)
  size_t used;              // Count of occupied slots (including tombstones)
  size_t size;              // Power of two, or 0 if unallocated
};

// Round up to the next power of two, or return the minimum size if n is
// less than that.
static size_t
bxr_hashmap_round_up_pow2_(size_t n)
{
  size_t c = BXR_HASHMAP_MIN_SIZE;

  while (c < n)
    c <<= 1;
  return c;
}

static bool
bxr_hashmap_alloc_arrays_(bxr_hashmap_t *out, size_t size)
{
  BXR_ASSERT(out);

  BXR_CALLOC(out->state, size, sizeof(bxr_hashmap_slot_e));
  BXR_ALLOC(out->hashes, size * sizeof(Uint64));
  BXR_ALLOC(out->keys, size * sizeof(Uint64));
  BXR_ALLOC(out->values, size * sizeof(hashmap_bucket_t));

  bool is_ok = true;
  is_ok &= out->state != NULL;
  is_ok &= out->hashes != NULL;
  is_ok &= out->keys != NULL;
  is_ok &= out->values != NULL;

  if (!is_ok) {
    BXR_FREE(out->state);
    BXR_FREE(out->hashes);
    BXR_FREE(out->keys);
    BXR_FREE(out->values);
  }

  out->size  = size;
  out->count = 0;
  out->used  = 0;

  return is_ok;
}

static size_t
bxr_hashmap_probe_(const bxr_hashmap_t *hashmap,
                   Uint64 key,
                   uint64_t h,
                   bool *found)
{
  // Power of two size allows to use a mask instead of modulo for wrapping.
  const size_t mask = hashmap->size - 1;

  size_t i          = (size_t)h & mask;
  size_t first_tomb = (size_t)-1;

  for (;;) {
    uint8_t s = hashmap->state[i];

    if (s == BXR_HASHMAP_SLOT_EMPTY) {
      *found = false;

      // If we found a tombstone, return its index for reuse; otherwise, return
      // the empty slot index.
      return (first_tomb != (size_t)-1) ? first_tomb : i;
    }

    // If the slot is occupied and the hash and key match, we found the key.
    if (s == BXR_HASHMAP_SLOT_OCCUPIED && hashmap->hashes[i] == h
        && hashmap->keys[i] == key) {
      *found = true;
      return i;
    }

    // If the slot is a tombstone and we haven't recorded a tombstone index yet,
    // record this index for potential reuse.
    if (s == BXR_HASHMAP_SLOT_TOMBSTONE && first_tomb == (size_t)-1) {
      first_tomb = i;
    }

    i = (i + 1) & mask; // linear probing with wrap-around
  }
}

static bool
bxr_hashmap_rehash_(bxr_hashmap_t *map, size_t new_cap)
{
  bxr_hashmap_t fresh;
  BXR_MEMSET(&fresh, 0, sizeof fresh);

  if (!bxr_hashmap_alloc_arrays_(&fresh, new_cap))
    return false;

  // Power of two size allows to use a mask instead of modulo for wrapping.
  const size_t mask = new_cap - 1;
  for (size_t i = 0; i < map->size; ++i) {

    // Skip empty slots and tombstones; only rehash occupied slots.
    if (map->state[i] != BXR_HASHMAP_SLOT_OCCUPIED)
      continue; /* tombstones dropped here */

    // Rehash the key and find a new slot in the fresh hashmap.
    uint64_t h = map->hashes[i];
    size_t j   = (size_t)h & mask;
    while (fresh.state[j] == BXR_HASHMAP_SLOT_OCCUPIED)
      j = (j + 1) & mask;

    // Move the key-value pair to the new slot in the fresh hashmap.
    fresh.state[j]  = BXR_HASHMAP_SLOT_OCCUPIED;
    fresh.hashes[j] = h;
    fresh.keys[j]   = map->keys[i];
    fresh.values[j] = map->values[i];
  }

  fresh.count = map->count;
  fresh.used  = map->count;

  BXR_FREE(map->state);
  BXR_FREE(map->hashes);
  BXR_FREE(map->keys);
  BXR_FREE(map->values);

  *map = fresh;

  return true;
}

// splitmix64 fast and low collision hash function
static uint64_t
bxr_hashmap_hash_(Uint64 key)
{
  key += 0x9E3779B97F4A7C15ull;
  key = (key ^ (key >> 30)) * 0xBF58476D1CE4E5B9ull;
  key = (key ^ (key >> 27)) * 0x94D049BB133111EBull;
  return key ^ (key >> 31);
}

bxr_hashmap_t *
bxr_hashmap_create(size_t initial_size)
{
  if (initial_size < BXR_HASHMAP_MIN_SIZE) {
    initial_size = BXR_HASHMAP_MIN_SIZE;
  }

  bxr_hashmap_t *map = NULL;
  BXR_NEW0(map);

  bool is_ok = bxr_hashmap_alloc_arrays_(
      map,
      bxr_hashmap_round_up_pow2_(initial_size ? initial_size
                                              : BXR_HASHMAP_MIN_SIZE));

  if (is_ok) {
    return map;
  }

  return NULL;
}

bool
bxr_hashmap_push(bxr_hashmap_t *map, Uint32 key, Uint32 value)
{
  BXR_ASSERT(map);

  if (map->used + 1 >= (map->size * BXR_HASHMAP_LOAD_NUMERATOR)
                           / BXR_HASHMAP_LOAD_DENOMINATOR) {
    if (!bxr_hashmap_rehash_(
            map, map->size ? map->size * 2 : BXR_HASHMAP_MIN_SIZE)) {
      return false;
    }
  }

  Uint64 hash = bxr_hashmap_hash_(key);
  bool found;
  size_t i = bxr_hashmap_probe_(map, key, hash, &found);

  if (!found) {
    if (map->state[i] != BXR_HASHMAP_SLOT_TOMBSTONE) {
      map->used++;
    }

    map->state[i]        = BXR_HASHMAP_SLOT_OCCUPIED;
    map->hashes[i]       = hash;
    map->keys[i]         = key;
    map->values[i].ids   = NULL;
    map->values[i].count = 0;
    map->values[i].size  = 0;
    map->count++;
  }

  hashmap_bucket_t *bucket = &map->values[i];
  if (bucket->count == bucket->size) {
    size_t new_size = bucket->size ? bucket->size * 2 : 4;

    BXR_RESIZE(bucket->ids, new_size * sizeof(Uint32));
    if (!bucket->ids) {
      return false;
    }

    bucket->size = new_size;
  }

  bucket->ids[bucket->count++] = value;
  return true;
}

bool
bxr_hashmap_remove(bxr_hashmap_t *map, Uint32 key, Uint32 value)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    return false;
  }

  bool found = false;
  size_t i   = bxr_hashmap_probe_(map, key, bxr_hashmap_hash_(key), &found);
  if (!found) {
    return false;
  }

  hashmap_bucket_t *bucket = &map->values[i];

  // Find the value in the bucket and remove it
  for (size_t j = 0; j < bucket->count; ++j) {
    if (bucket->ids[j] == value) {
      // Move the last element to the current position
      bucket->ids[j] = bucket->ids[bucket->count - 1];
      bucket->count--;
      // no break here, as we want to remove all instances of the value
    }
  }

  // If the bucket is empty, mark the slot as a tombstone
  if (bucket->count == 0) {
    map->state[i] = BXR_HASHMAP_SLOT_TOMBSTONE;
    map->count--;
  }

  return true;
}

bool
bxr_hashmap_remove_bucket(bxr_hashmap_t *map, Uint32 key)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    return false;
  }

  bool found = false;
  size_t i   = bxr_hashmap_probe_(map, key, bxr_hashmap_hash_(key), &found);
  if (!found) {
    return false;
  }

  BXR_FREE(map->values[i].ids);
  map->values[i].count = 0;
  map->values[i].size  = 0;

  map->state[i] = BXR_HASHMAP_SLOT_TOMBSTONE;
  map->count--;

  return true;
}

bool
bxr_hashmap_get_bucket(bxr_hashmap_t *map,
                       Uint32 key,
                       hashmap_bucket_t *out_values)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    return false;
  }

  bool found = false;
  size_t i   = bxr_hashmap_probe_(map, key, bxr_hashmap_hash_(key), &found);
  if (!found) {
    return false;
  }

  if (out_values) {
    *out_values = map->values[i];
  }

  return true;
}

void
bxr_hashmap_destroy(bxr_hashmap_t *map)
{
  if (!map) {
    return;
  }

  BXR_FREE(map->state);
  BXR_FREE(map->hashes);
  BXR_FREE(map->keys);
  BXR_FREE(map->values);
  BXR_FREE(map);
}
