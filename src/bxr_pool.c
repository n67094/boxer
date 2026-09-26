#include "bxr_pool.h"

bxr_pool_t *
bxr_pool_create(size_t number_of_slots)
{
  bxr_pool_t *p = NULL;
  BXR_NEW(p);
  if (!p) {
    return NULL;
  }

  p->pool = SDL_GPCreatePool(number_of_slots);
  if (!p->pool) {
    BXR_LOG_ERROR("Failed to create pool with %zu slots", number_of_slots);
    BXR_FREE(p);
    return NULL;
  }

  return p;
}

void
bxr_pool_destroy(bxr_pool_t *pool)
{
  if (!pool) {
    return;
  }

  SDL_GPDestroyPool(pool->pool);
  BXR_FREE(pool);
}

int
bxr_pool_aquire_slot(bxr_pool_t *pool)
{
  if (!pool) {
    return BXR_POOL_INVALID_SLOT;
  }
  return SDL_GPAcquirePoolSlot(pool->pool);
}

void
bxr_pool_release_slot(bxr_pool_t *pool, size_t slot_index)
{
  if (!pool) {
    return;
  }
  SDL_GPReleasePoolSlot(pool->pool, slot_index);
}

Uint32
bxr_pool_generate_id(bxr_pool_t *pool, size_t slot_index)
{
  if (!pool) {
    return 0;
  }
  return SDL_GPGeneratePoolID(pool->pool, slot_index);
}

bool
bxr_pool_is_valid_id(bxr_pool_t *pool, Uint32 id)
{
  return SDL_GPPoolIsValidID(pool->pool, id);
}

int
bxr_pool_get_slot_from_id(bxr_pool_t *pool, Uint32 id)
{
  return SDL_GPPoolIDToSlot(pool->pool, id);
}
