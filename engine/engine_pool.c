#include "engine_pool.h"
#include "engine_mem.h"

engine_pool_t *
engine_pool_make(size_t number_of_ids)
{
  engine_pool_t *pool = NULL;
  ENGINE_NEW(pool);

  pool->size = number_of_ids + 1; // since the 0 index is used for invalid id
  pool->queue_top = 0;

  ENGINE_CALLOC(pool->counters, pool->size, sizeof(Uint32));

  // Since id 0 is reserved for invalid id, we only need number_of_ids slots
  ENGINE_CALLOC(pool->free_queue, number_of_ids, sizeof(int));

  for (int i = pool->size - 1; i >= 1; --i) {
    pool->free_queue[pool->queue_top++] = i;
    pool->counters[i]                   = 0;
  }

  return pool;
}

void
engine_pool_destroy(engine_pool_t *pool)
{
  ENGINE_FREE(pool->counters);
  ENGINE_FREE(pool->free_queue);
  ENGINE_FREE(pool);
}

int
engine_pool_acquire_slot(engine_pool_t *pool)
{
  SDL_assert(pool);
  SDL_assert(pool->free_queue);

  if (pool->queue_top > 0) {
    int slot = pool->free_queue[--pool->queue_top];
    return slot;
  } else {
    return _ENGINE_INVALID_SLOT; // No more slots available
  }
}

void
engine_pool_release_slot(engine_pool_t *pool, int slot)
{
  SDL_assert(slot > _ENGINE_INVALID_SLOT && slot < (int)pool->size);
  SDL_assert(pool);
  SDL_assert(pool->free_queue);
  SDL_assert(pool->queue_top < (int)pool->size);

#ifdef ENGINE_DEBUG
  for (int i = 0; i < pool->queue_top; ++i) {
    SDL_assert(pool->free_queue[i] != slot);
  }
#endif
  pool->free_queue[pool->queue_top++] = slot;

  SDL_assert(pool->queue_top <= (int)pool->size);
}

Uint32
engine_pool_gen_id(engine_pool_t *pool, int slot_index)
{
  SDL_assert(slot_index > _ENGINE_INVALID_SLOT && slot_index < (int)pool->size);
  SDL_assert(pool);
  SDL_assert(pool->counters);

  uint32_t counter = ++pool->counters[slot_index]; // increment generation

  Uint32 id
      = (counter << _ENGINE_SLOT_SHIFT) | (slot_index & _ENGINE_SLOT_MASK);

  return id;
}

int
engine_pool_id_to_slot(Uint32 id)
{
  int slot_index = (int)(id & _ENGINE_SLOT_MASK);
  return slot_index;
}
