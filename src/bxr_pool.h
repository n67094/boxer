/**
 * File `bxr_pool.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Pool for Managing Resources
 */

#ifndef BXR_POOL_H_
#define BXR_POOL_H_

#include <SDL_gp.h>

#define BXR_POOL_INVALID SDL_GP_POOL_INVALID_SLOT
#define BXR_POOL_SLOT_SHIFT SDL_GP_POOL_SLOT_SHIFT 16
#define BXR_POOL_SLOT_MASK                                                     \
  SDL_GP_POOL_SLOT_MASK((1 << SDL_GP_POOL_SLOT_SHIFT) - 1)

typedef SDL_GPPool bxr_pool_t;

/**
 * Create a pool.
 *
 * `number_of_slots` is the maximum number of slots that can be acquired.
 *
 * `return` a pointer to the created pool, or NULL if the pool could not be
 * created.
 */
bxr_pool_t *bxr_make_pool(size_t number_of_slots);

/**
 * Destroy a pool.
 *
 * `resource` is the pool to destroy.
 */
void bxr_destroy_pool(bxr_pool_t *resource);

/**
 * Acquire a slot from the pool.
 *
 * `resource` is the pool to acquire a slot from.
 *
 * `return` the index of the acquired slot, or BXR_POOL_INVALID if no slots are
 * available.
 */
int bxr_aquire_pool_slot(bxr_pool_t *resource);

/**
 * Release a slot back to the pool.
 *
 * `resource` is the pool to release the slot to.
 *
 * `slot_index` is the index of the slot to release.
 */
void bxr_release_pool_slot(bxr_pool_t *resource, int slot_index);

/*
 * Generate a unique id for a slot in the pool.
 *
 * `resource` is the pool to generate the id for.
 *
 * `slot_index` is the index of the slot to generate the id for.
 */
Uint32 bxr_generate_pool_id(bxr_pool_t *resource, int slot_index);

/*
 * Extract the slot index from a generated id.
 *
 * `id` is the id to extract the slot index from.
 *
 * `return` the slot index extracted from the id.
 */
int bxr_pool_id_to_slot(Uint32 id);

#endif // BXR_POOL_H_
