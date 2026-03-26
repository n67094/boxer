/**
 * @file bxr_pool.h
 *
 * @brief Pool structure definition for managing reusable IDs.
 *
 * @details The pool allow efficient allocation and deallocation of unique IDs
 * by reusing freed slots and maintaining generation counters to avoid ID reuse
 * issues.
 *
 * Usage:
 *
 * 1. Create a pool using `bxr_pool_make()`.
 * 2. Acquire a slot using `bxr_pool_acquire_slot()`.
 * 3. Allocate an ID for the slot using `bxr_pool_alloc_slot()`.
 * 4. Release the slot back to the pool using `bxr_pool_release_slot()
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_POOL_H_
#define BXR_POOL_H_

#include <SDL3/SDL.h>

#define _BXR_INVALID_SLOT 0
#define _BXR_SLOT_SHIFT 16
#define _BXR_SLOT_MASK ((1 << _BXR_SLOT_SHIFT) - 1)

typedef struct bxr_pool_s
{
  size_t size;
  int queue_top;
  Uint32 *counters; // incrementing generation counters for each slot
  int *free_queue;
} bxr_pool_t;

bxr_pool_t *bxr_pool_make(size_t number_of_ids);
void bxr_pool_destroy(bxr_pool_t *pool);

int bxr_pool_acquire_slot(bxr_pool_t *pool);
void bxr_pool_release_slot(bxr_pool_t *pool, int slot_index);
Uint32 bxr_pool_gen_id(bxr_pool_t *pool, int slot_index);
int bxr_pool_id_to_slot(Uint32 id);

#endif // BXR_POOL_H_
