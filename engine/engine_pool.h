/**
 * @file engine_pool.h
 *
 * @brief Pool structure definition for managing reusable IDs.
 *
 * @details The pool allow efficient allocation and deallocation of unique IDs
 * by reusing freed slots and maintaining generation counters to avoid ID reuse
 * issues.
 *
 * Usage:
 *
 * 1. Create a pool using `engine_pool_make()`.
 * 2. Acquire a slot using `engine_pool_acquire_slot()`.
 * 3. Allocate an ID for the slot using `engine_pool_alloc_slot()`.
 * 4. Release the slot back to the pool using `engine_pool_release_slot()
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_POOL_H_
#define ENGINE_POOL_H_

#include <SDL3/SDL.h>

#define _ENGINE_INVALID_SLOT 0
#define _ENGINE_SLOT_SHIFT 16
#define _ENGINE_SLOT_MASK ((1 << _ENGINE_SLOT_SHIFT) - 1)

typedef struct engine_pool_s
{
  size_t size;
  int queue_top;
  Uint32 *counters; // incrementing generation counters for each slot
  int *free_queue;
} engine_pool_t;

engine_pool_t *engine_pool_make(size_t number_of_ids);
void engine_pool_destroy(engine_pool_t *pool);

int engine_pool_acquire_slot(engine_pool_t *pool);
void engine_pool_release_slot(engine_pool_t *pool, int slot_index);
Uint32 engine_pool_gen_id(engine_pool_t *pool, int slot_index);
int engine_pool_id_to_slot(Uint32 id);

#endif // ENGINE_POOL_H_
