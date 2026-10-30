/**
 * File `bxr_sparse_set.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Sparse Set
 *
 * Set of integer handles (e.g. entity ids) with O(1) insert, remove and lookup.
 *
 * The set is backed by two arrays:
 *
 * - `Sparse array`: indexed by handle, stores the handle position in the dense
 *   array.
 * - `Dense array`: stores the handles contiguously, for fast iteration.
 *
 * Both arrays are sized by the largest handle inserted, not by the number of
 * handles, so handles should be small, densely allocated integers.
 *
 * Typical usage: create the set, insert / remove / check handles, iterate the
 * dense array, destroy the set.
 */

#ifndef BXR_SPARSE_SET_H_
#define BXR_SPARSE_SET_H_

#include <SDL3/SDL.h>

typedef struct bxr_sparse_set_s bxr_sparse_set_t;

/**
 * Create an empty sparse set.
 *
 * The set grows automatically when a handle `>=` the capacity is inserted.
 *
 * `default_capacity` initial capacity, handles in `[0, default_capacity)` can
 * be inserted without resizing. Must be greater than 0.
 *
 * `return` the new set, or NULL if `default_capacity` is 0 or allocation
 * failed.
 *
 * **Note**: the caller must free the result with `bxr_sparse_set_destroy()`.
 */
bxr_sparse_set_t *bxr_sparse_set_create(size_t default_capacity);

/**
 * Destroy a sparse set and free its memory.
 *
 * `set` the set to destroy, may be NULL.
 */
void bxr_sparse_set_destroy(bxr_sparse_set_t *set);

/**
 * Insert a handle into the set.
 *
 * `set` the set. Must not be NULL.
 * `handle` the handle to insert.
 *
 * `return` true on success, false if the handle is already in the set or if
 * resizing failed.
 */
bool bxr_sparse_set_insert(bxr_sparse_set_t *set, size_t handle);

/**
 * Remove a handle from the set.
 *
 * `set` the set. Must not be NULL.
 * `handle` the handle to remove.
 *
 * `return` true on success, false if the handle is not in the set.
 */
bool bxr_sparse_set_remove(bxr_sparse_set_t *set, size_t handle);

/**
 * Check whether a handle is in the set.
 *
 * `set` the set. Must not be NULL.
 * `handle` the handle to look up.
 * `found` optional output, may be NULL. Receives the handle index in the dense
 * array, or `SIZE_MAX` if the handle is not in the set.
 *
 * `return` true if the handle is in the set, false otherwise.
 */
bool bxr_sparse_set_contains(const bxr_sparse_set_t *set,
                             size_t handle,
                             size_t *found);

/**
 * Get the capacity of the set.
 *
 * `set` the set. Must not be NULL.
 *
 * `return` the capacity, handles in `[0, capacity)` can be inserted without
 * resizing.
 */
size_t bxr_sparse_set_capacity(const bxr_sparse_set_t *set);

/**
 * Get the number of handles in the set.
 *
 * `set` the set. Must not be NULL.
 *
 * `return` the number of handles, also the number of valid entries in the
 * dense array.
 */
size_t bxr_sparse_set_count(const bxr_sparse_set_t *set);

/**
 * Remove all handles from the set.
 *
 * The capacity and allocated memory are kept for future insertions.
 *
 * `set` the set. Must not be NULL.
 */
void bxr_sparse_set_clear(bxr_sparse_set_t *set);

/**
 * Get the dense array of the set, to iterate over its handles.
 *
 * Only the first `bxr_sparse_set_count()` entries are valid. The order is
 * unspecified and can changes.
 *
 * `set` the set. Must not be NULL.
 *
 * `return` the dense array, owned by the set.
 *
 * **Note**: the pointer is invalidated by `bxr_sparse_set_insert()` (the array
 * may be reallocated) and `bxr_sparse_set_destroy()`. Do not free it.
 */
const size_t *bxr_sparse_set_get_dense(const bxr_sparse_set_t *set);

#endif // BXR_SPARSE_SET_H_
