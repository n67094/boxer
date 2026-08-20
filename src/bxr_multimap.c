#include "bxr_multimap.h"
#include "bxr_assert.h"
#include "bxr_mem.h"
#include "bxr_string.h"

typedef struct bxr_multimap_bucket_s
{
  Uint32 *values; // array of resource data
  size_t count;
  size_t size; // Power of two, or 0 if unallocated
} bxr_multimap_bucket_t;

struct bxr_multimap_s
{
  bxr_multimap_slot_e *state;
  Uint64 *hashes; // Avoids rehashing keys
  char **keys;
  bxr_multimap_bucket_t *buckets; // Map keys to a bucket of ids
  size_t count; // Count of occupied slots (excluding tombstones)
  size_t used;  // Count of occupied slots (including tombstones)
  size_t size;  // Power of two, or 0 if unallocated
};

// Round up to the next power of two, or return the minimum size if n is
// less than that.
static size_t
bxr_multimap_round_up_pow2_(size_t n)
{
  size_t c = BXR_MULTIMAP_MIN_SIZE;

  while (c < n)
    c <<= 1;
  return c;
}

static bool
bxr_multimap_alloc_arrays_(bxr_multimap_t *out, size_t size)
{
  BXR_ASSERT(out);

  BXR_CALLOC(out->state, size, sizeof(bxr_multimap_slot_e));
  BXR_ALLOC(out->hashes, size * sizeof(Uint64));
  BXR_ALLOC(out->keys, size * sizeof(char *));
  BXR_ALLOC(out->buckets, size * sizeof(bxr_multimap_bucket_t));

  bool is_ok = true;
  is_ok &= out->state != NULL;
  is_ok &= out->hashes != NULL;
  is_ok &= out->keys != NULL;
  is_ok &= out->buckets != NULL;

  if (!is_ok) {
    BXR_FREE(out->state);
    BXR_FREE(out->hashes);
    BXR_FREE(out->keys);
    BXR_FREE(out->buckets);
  }

  out->size  = size;
  out->count = 0;
  out->used  = 0;

  return is_ok;
}

static size_t
bxr_multimap_probe_(const bxr_multimap_t *multimap,
                    const char *key,
                    size_t key_len,
                    uint64_t h,
                    bool *found)
{
  // Power of two size allows to use a mask instead of modulo for wrapping.
  const size_t mask = multimap->size - 1;

  size_t i          = (size_t)h & mask;
  size_t first_tomb = (size_t)-1;

  for (;;) {
    uint8_t s = multimap->state[i];

    if (s == BXR_MULTIMAP_SLOT_EMPTY) {
      *found = false;

      // If we found a tombstone, return its index for reuse; otherwise, return
      // the empty slot index.
      return (first_tomb != (size_t)-1) ? first_tomb : i;
    }

    // If the slot is occupied and the hash and key match, we found the key.
    if (s == BXR_MULTIMAP_SLOT_OCCUPIED && multimap->hashes[i] == h
        && bxr_strncmp(multimap->keys[i], key, key_len) == 0) {
      *found = true;
      return i;
    }

    // If the slot is a tombstone and we haven't recorded a tombstone index yet,
    // record this index for potential reuse.
    if (s == BXR_MULTIMAP_SLOT_TOMBSTONE && first_tomb == (size_t)-1) {
      first_tomb = i;
    }

    i = (i + 1) & mask; // linear probing with wrap-around
  }
}

static bool
bxr_multimap_rehash_(bxr_multimap_t *map, size_t new_cap)
{
  bxr_multimap_t fresh;
  BXR_MEMSET(&fresh, 0, sizeof fresh);

  if (!bxr_multimap_alloc_arrays_(&fresh, new_cap))
    return false;

  // Power of two size allows to use a mask instead of modulo for wrapping.
  const size_t mask = new_cap - 1;
  for (size_t i = 0; i < map->size; ++i) {

    // Skip empty slots and tombstones; only rehash occupied slots.
    if (map->state[i] != BXR_MULTIMAP_SLOT_OCCUPIED)
      continue; /* tombstones dropped here */

    // Rehash the key and find a new slot in the fresh multimap.
    uint64_t h = map->hashes[i];
    size_t j   = (size_t)h & mask;
    while (fresh.state[j] == BXR_MULTIMAP_SLOT_OCCUPIED)
      j = (j + 1) & mask;

    // Move the key-value pair to the new slot in the fresh multimap.
    fresh.state[j]   = BXR_MULTIMAP_SLOT_OCCUPIED;
    fresh.hashes[j]  = h;
    fresh.keys[j]    = map->keys[i];
    fresh.buckets[j] = map->buckets[i];
  }

  fresh.count = map->count;
  fresh.used  = map->count;

  BXR_FREE(map->state);
  BXR_FREE(map->hashes);
  BXR_FREE(map->keys);
  BXR_FREE(map->buckets);

  *map = fresh;

  return true;
}

// FNV-1a fast and low collision hash function
static Uint64
bxr_multimap_hash_(const char *key, size_t len)
{
  Uint64 h = 0xCBF29CE484222325ull;
  for (size_t i = 0; i < len; ++i) {
    h ^= (Uint64)(unsigned char)key[i];
    h *= 0x100000001B3ull;
  }
  return h;
}

bxr_multimap_t *
bxr_multimap_create(size_t initial_size)
{
  if (initial_size < BXR_MULTIMAP_MIN_SIZE) {
    initial_size = BXR_MULTIMAP_MIN_SIZE;
  }

  bxr_multimap_t *map = NULL;
  BXR_NEW0(map);

  bool is_ok = bxr_multimap_alloc_arrays_(
      map,
      bxr_multimap_round_up_pow2_(initial_size ? initial_size
                                               : BXR_MULTIMAP_MIN_SIZE));

  if (is_ok) {
    return map;
  }

  return NULL;
}

bool
bxr_multimap_push(bxr_multimap_t *map, const char *key, Uint32 value)
{
  BXR_ASSERT(map);

  // Rehash if the load factor exceeds the threshold (used + 1) / size >= 0.75
  if (map->used + 1 >= (map->size * BXR_MULTIMAP_LOAD_NUMERATOR)
                           / BXR_MULTIMAP_LOAD_DENOMINATOR) {
    if (!bxr_multimap_rehash_(
            map, map->size ? map->size * 2 : BXR_MULTIMAP_MIN_SIZE)) {
      return false;
    }
  }

  // Compute the hash of the key and probe for its position in the multimap.
  Uint64 hash = bxr_multimap_hash_(key, bxr_strlen(key));
  bool found;
  size_t i = bxr_multimap_probe_(map, key, bxr_strlen(key), hash, &found);

  // If the key was not found, we need to insert it into the multimap.
  if (!found) {
    if (map->state[i] != BXR_MULTIMAP_SLOT_TOMBSTONE) {
      map->used++;
    }

    char *key_copy = bxr_strdup(key);
    if (!key_copy) {
      return false;
    }

    map->state[i]          = BXR_MULTIMAP_SLOT_OCCUPIED;
    map->hashes[i]         = hash;
    map->keys[i]           = key_copy;
    map->buckets[i].values = NULL;
    map->buckets[i].count  = 0;
    map->buckets[i].size   = 0;
    map->count++;
  }

  // Now we can safely add the value to the bucket associated with the key.
  bxr_multimap_bucket_t *bucket = &map->buckets[i];

  // Resize the bucket if it's full. We double the size of the bucket each time
  if (bucket->count == bucket->size) {
    size_t new_size = bucket->size ? bucket->size * 2 : 4;

    BXR_RESIZE(bucket->values, new_size * sizeof(Uint32));
    if (!bucket->values) {

      // If we failed to allocate memory for the bucket, we need to clean up the
      // key and mark the slot as a tombstone.
      if (!found) {
        BXR_FREE(map->keys[i]);
        map->keys[i]  = NULL;
        map->state[i] = BXR_MULTIMAP_SLOT_TOMBSTONE;
        map->count--;
      }

      return false;
    }

    bucket->size = new_size;
  }

  for (size_t j = 0; j < bucket->count; ++j) {
    if (bucket->values[j] == value) {
      // Value already exists in the bucket, no need to add it again
      return true;
    }
  }

  bucket->values[bucket->count++] = value;
  return true;
}

bool
bxr_multimap_remove_value(bxr_multimap_t *map, const char *key, Uint32 value)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    return false;
  }

  bool found = false;
  size_t i   = bxr_multimap_probe_(map,
                                   key,
                                   bxr_strlen(key),
                                   bxr_multimap_hash_(key, bxr_strlen(key)),
                                   &found);
  if (!found) {
    return false;
  }

  bxr_multimap_bucket_t *bucket = &map->buckets[i];

  // Find the value in the bucket and remove it
  for (size_t j = 0; j < bucket->count; ++j) {

    if (bucket->values[j] == value) {
      // Move the last element to the current position
      bucket->values[j] = bucket->values[bucket->count - 1];
      bucket->count--;
      break;
    }
  }

  // If the bucket is empty, mark the slot as a tombstone
  if (bucket->count == 0) {
    map->state[i] = BXR_MULTIMAP_SLOT_TOMBSTONE;
    map->count--;
  }

  return true;
}

bool
bxr_multimap_remove(bxr_multimap_t *map, const char *key)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    return false;
  }

  bool found = false;
  size_t i   = bxr_multimap_probe_(map,
                                   key,
                                   bxr_strlen(key),
                                   bxr_multimap_hash_(key, bxr_strlen(key)),
                                   &found);
  if (!found) {
    return false;
  }

  BXR_FREE(map->buckets[i].values);
  map->buckets[i].values = NULL;
  map->buckets[i].count  = 0;
  map->buckets[i].size   = 0;

  BXR_FREE(map->keys[i]);
  map->keys[i] = NULL;

  map->state[i] = BXR_MULTIMAP_SLOT_TOMBSTONE;
  map->count--;

  return true;
}

size_t
bxr_multimap_get_values(bxr_multimap_t *map,
                        const char *key,
                        Uint32 **out_values)
{
  BXR_ASSERT(map);

  if (map->count == 0 || map->size == 0) {
    if (out_values) {
      *out_values = NULL;
    }

    return 0;
  }

  bool found = false;
  size_t i   = bxr_multimap_probe_(map,
                                   key,
                                   bxr_strlen(key),
                                   bxr_multimap_hash_(key, bxr_strlen(key)),
                                   &found);
  if (!found) {
    return false;
  }

  if (out_values) {
    BXR_ALLOC(*out_values, map->buckets[i].count * sizeof(Uint32));
    BXR_MEMCPY(*out_values,
               map->buckets[i].values,
               map->buckets[i].count * sizeof(Uint32));
  }

  return map->buckets[i].count;
}

size_t
bxr_multimap_count(bxr_multimap_t *map)
{
  BXR_ASSERT(map);
  return map->count;
}

void
bxr_multimap_destroy(bxr_multimap_t *map)
{
  if (!map) {
    return;
  }

  for (size_t i = 0; i < map->size; ++i) {
    if (map->state[i] == BXR_MULTIMAP_SLOT_OCCUPIED) {
      BXR_FREE(map->keys[i]);
      BXR_FREE(map->buckets[i].values);
    }
  }

  BXR_FREE(map->state);
  BXR_FREE(map->hashes);
  BXR_FREE(map->keys);
  BXR_FREE(map->buckets);
  BXR_FREE(map);
}
