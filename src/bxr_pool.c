#include "bxr_mem.h"
#include "bxr_pool.h"

bxr_pool_t *
bxr_pool_make(size_t number_of_ids)
{
  bxr_pool_t *pool = NULL;
  BXR_NEW(pool);

  pool->size = number_of_ids + 1; // since the 0 index is used for invalid id
  pool->queue_top = 0;

  BXR_CALLOC(pool->counters, pool->size, sizeof(Uint32));

  // Since id 0 is reserved for invalid id, we only need number_of_ids slots
  BXR_CALLOC(pool->free_queue, number_of_ids, sizeof(int));

  for (int i = pool->size - 1; i >= 1; --i) {
    pool->free_queue[pool->queue_top++] = i;
    pool->counters[i]                   = 0;
  }

  return pool;
}

void
bxr_pool_destroy(bxr_pool_t *pool)
{
  BXR_FREE(pool->counters);
  BXR_FREE(pool->free_queue);
  BXR_FREE(pool);
}

int
bxr_pool_acquire_slot(bxr_pool_t *pool)
{
  SDL_assert(pool);
  SDL_assert(pool->free_queue);

  if (pool->queue_top > 0) {
    int slot = pool->free_queue[--pool->queue_top];
    return slot;
  } else {
    return _BXR_INVALID_SLOT; // No more slots available
  }
}

void
bxr_pool_release_slot(bxr_pool_t *pool, int slot)
{
  SDL_assert(slot > _BXR_INVALID_SLOT && slot < (int)pool->size);
  SDL_assert(pool);
  SDL_assert(pool->free_queue);
  SDL_assert(pool->queue_top < (int)pool->size);

#ifdef BXR_DEBUG
  for (int i = 0; i < pool->queue_top; ++i) {
    SDL_assert(pool->free_queue[i] != slot);
  }
#endif
  pool->free_queue[pool->queue_top++] = slot;

  SDL_assert(pool->queue_top <= (int)pool->size);
}

Uint32
bxr_pool_gen_id(bxr_pool_t *pool, int slot_index)
{
  SDL_assert(slot_index > _BXR_INVALID_SLOT && slot_index < (int)pool->size);
  SDL_assert(pool);
  SDL_assert(pool->counters);

  uint32_t counter = ++pool->counters[slot_index]; // increment generation

  Uint32 id = (counter << _BXR_SLOT_SHIFT) | (slot_index & _BXR_SLOT_MASK);

  return id;
}

int
bxr_pool_id_to_slot(Uint32 id)
{
  int slot_index = (int)(id & _BXR_SLOT_MASK);
  return slot_index;
}
