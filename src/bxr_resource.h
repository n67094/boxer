/**
 * TODO re-rename as pool
 * @file bxr_resource.h
 *
 * @brief Pool structure definition for managing reusable IDs (resource
 * management).
 *
 * @details The resource allow efficient allocation and deallocation of unique
 * IDs by reusing freed slots and maintaining generation counters to avoid ID
 * reuse issues.
 *
 * Usage:
 *
 * 1. Create a resource using `bxr_resource_make()`.
 * 2. Acquire a slot using `bxr_resource_acquire_slot()`.
 * 3. Allocate an ID for the slot using `bxr_resource_alloc_slot()`.
 * 4. Release the slot back to the resource using `bxr_resource_release_slot()
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_RESOURCE_H_
#define BXR_RESOURCE_H_

#include <SDL3/SDL.h>

#define _BXR_RESOURCE_INVALID_SLOT 0
#define _BXR_RESOURCE_SLOT_SHIFT 16
#define _BXR_RESOURCE_SLOT_MASK ((1 << _BXR_RESOURCE_SLOT_SHIFT) - 1)

typedef struct bxr_resource_s
{
  size_t size;      // total number of slots in the resource
  int queue_top;    // index of the top of the free slot stack
  Uint32 *counters; // incrementing generation counters for each slot
  int *free_queue;  // stack of free slot indices for efficient reuse
} bxr_resource_t;

bxr_resource_t *bxr_resource_make(size_t number_of_ids);
void bxr_resource_destroy(bxr_resource_t *resource);

int bxr_resource_acquire_slot(bxr_resource_t *resource);
void bxr_resource_release_slot(bxr_resource_t *resource, int slot_index);
Uint32 bxr_resource_gen_id(bxr_resource_t *resource, int slot_index);
int bxr_resource_id_to_slot(Uint32 id);

#endif // BXR_POOL_H_
