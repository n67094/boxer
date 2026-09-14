#include "bxr_pool.h"

bxr_pool_t *
bxr_pool_create(size_t number_of_slots)
{
  return (bxr_pool_t *)SDL_GPCreatePool(number_of_slots);
}

void
bxr_pool_destroy(bxr_pool_t *pool)
{
  SDL_GPDestroyPool((SDL_GPPool *)pool);
  BXR_FREE(pool);
}

int
bxr_pool_aquire_slot(bxr_pool_t *pool)
{
  return SDL_GPAcquirePoolSlot((SDL_GPPool *)pool);
}

void
bxr_pool_release_slot(bxr_pool_t *pool, size_t slot_index)
{
  SDL_GPReleasePoolSlot((SDL_GPPool *)pool, slot_index);
}

Uint32
bxr_pool_generate_id(bxr_pool_t *pool, size_t slot_index)
{
  return SDL_GPGeneratePoolId((SDL_GPPool *)pool, slot_index);
}

int
bxr_pool_get_slot_from_id(bxr_pool_t *pool, Uint32 id)
{
  return SDL_GPPoolIdToSlot(id);
}
