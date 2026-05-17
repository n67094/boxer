#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_mem.h"
#include "bxr_sparse_set.h"

/**
 * Data structure for sparse set.
 *
 * `sparse` store indices of handles in the dense array.
 * `dense` store the actual handles in a contiguous manner.
 * `count` is the number of handles stored in the sparse set.
 * `capacity` is the total capacity of the sparse set (the max number of handles
 * that can be stored in the sparse set without resizing).
 */
struct bxr_sparse_set_s
{
  size_t *sparse;
  size_t *dense;
  size_t count;
  size_t capacity;
};

bxr_sparse_set_t *
bxr_sparse_set_create(size_t default_capacity)
{
  if (default_capacity == 0) {
    return NULL;
  }

  bxr_sparse_set_t *set = NULL;
  BXR_NEW(set);
  if (!set) {
    goto error;
  }

  set->dense = NULL;
  BXR_ALLOC(set->dense, default_capacity * sizeof(size_t));
  if (!set->dense) {
    bxr_sparse_set_destroy(set);
    goto error;
  }

  set->sparse = NULL;
  BXR_ALLOC(set->sparse, default_capacity * sizeof(size_t));
  if (!set->sparse) {
    bxr_sparse_set_destroy(set);
    goto error;
  }

  // Set to zero since it can be read before being written to.
  BXR_MEMSET(set->sparse, 0, default_capacity * sizeof(size_t));
  if (!set->sparse) {
    bxr_sparse_set_destroy(set);
    goto error;
  }

  set->count    = 0;
  set->capacity = default_capacity;

  return set;

error:
  BXR_FREE(set->dense);
  BXR_FREE(set->sparse);
  BXR_FREE(set);

  return NULL;
}

void
bxr_sparse_set_destroy(bxr_sparse_set_t *set)
{
  if (set) {
    BXR_FREE(set->dense);
    BXR_FREE(set->sparse);
  }

  BXR_FREE(set);
}

bool
bxr_sparse_set_insert(bxr_sparse_set_t *set, size_t handle)
{
  BXR_ASSERT(set);

  // Check if the handle already exists in the set
  if (bxr_sparse_set_contains(set, handle, NULL)) {
    return false;
  }

  // Double the capacity if the handle is out of bounds of the current capacity
  if (handle >= set->capacity) {

    size_t new_capacity = set->capacity;
    while (handle >= new_capacity) {
      new_capacity *= 2;
    }

    BXR_RESIZE(set->dense, new_capacity * sizeof(size_t));
    BXR_RESIZE(set->sparse, new_capacity * sizeof(size_t));

    set->capacity = new_capacity;
  }

  // Insert the handle into the set
  set->dense[set->count] = handle;
  set->sparse[handle]    = set->count;
  set->count++;

  return true;
}

bool
bxr_sparse_set_remove(bxr_sparse_set_t *set, size_t handle)
{
  BXR_ASSERT(set);

  // Check if the handle exists in the set
  if (!bxr_sparse_set_contains(set, handle, NULL)) {
    return false;
  }

  // Move the last handle to the position of the handle to remove
  Uint32 last_handle              = set->dense[set->count - 1];
  set->dense[set->sparse[handle]] = last_handle;

  // Make the last_handle value equal to the value of the remove
  // one.
  set->sparse[last_handle] = set->sparse[handle];
  set->count--;

  return true;
}

bool
bxr_sparse_set_contains(const bxr_sparse_set_t *set,
                        size_t handle,
                        size_t *found)
{
  BXR_ASSERT(set);

  if (handle < set->capacity && set->sparse[handle] < set->count
      && set->dense[set->sparse[handle]] == handle) {
    if (found) {
      *found = set->sparse[handle];
    }
    return true;
  } else {
    if (found) {
      *found = UINT32_MAX;
    }
    return false;
  }
}

size_t
bxr_sparse_set_capacity(const bxr_sparse_set_t *set)
{
  BXR_ASSERT(set);

  return set->capacity;
}

size_t
bxr_sparse_set_count(const bxr_sparse_set_t *set)
{
  BXR_ASSERT(set);

  return set->count;
}

void
bxr_sparse_set_clear(bxr_sparse_set_t *set)
{
  BXR_ASSERT(set);

  set->count = 0;
}

size_t *
bxr_sparse_set_get_dense(const bxr_sparse_set_t *set)
{
  BXR_ASSERT(set);

  return set->dense;
}
