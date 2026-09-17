#ifndef BXR_POOL_H_
#define BXR_POOL_H_

#include <boxer/bxr.h>

#include <SDL_gp.h>

typedef struct bxr_pool_s
{
  SDL_GPPool *pool;
} bxr_pool_t;

#define BXR_POOL_INVALID_SLOT SDL_GP_POOL_INVALID_SLOT

bxr_pool_t *bxr_pool_create(size_t number_of_slots);

void bxr_pool_destroy(bxr_pool_t *pool);

int bxr_pool_aquire_slot(bxr_pool_t *pool);

void bxr_pool_release_slot(bxr_pool_t *pool, size_t slot_index);

Uint32 bxr_pool_generate_id(bxr_pool_t *pool, size_t slot_index);

int bxr_pool_get_slot_from_id(Uint32 id);

#endif // BXR_POOL_H_
