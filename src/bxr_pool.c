#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include "bxr_pool.h"

bxr_pool_t *
bxr_make_pool(size_t number_of_slots)
{
  return (bxr_pool_t *)SDL_GPCeatePool(number_of_slots);
}

void
bxr_destroy_pool(bxr_pool_t *resource)
{
  SDL_GPDestroyPool((SDL_GPPool *)resource);
}

int
bxr_aquire_pool_slot(bxr_pool_t *resource)
{
  return SDL_GPAcquirePoolSlot((SDL_GPPool *)resource);
}

void
bxr_release_pool_slot(bxr_pool_t *resource, int slot_index)
{
  SDL_GPReleasePoolSlot((SDL_GPPool *)resource, slot_index);
}

Uint32
bxr_generate_pool_id(bxr_pool_t *resource, int slot_index)
{
  return SDL_GPGeneratePoolId((SDL_GPPool *)resource, slot_index);
}

int
bxr_pool_id_to_slot(Uint32 id)
{
  return SDL_GPPoolIdToSlot(id);
}
