#include "bxr_pool.h"
#include "bxr_error.h"
#include "bxr_log.h"
#include "bxr_mem.h"
#include "bxr_sdl_gp.h"

struct bxr_pool_s
{
  SDL_GPPool *pool;
};

bxr_pool_t *
bxr_pool_create(size_t number_of_slots)
{
  bxr_pool_t *p = NULL;
  BXR_NEW(p);
  if (!p) {
    bxr_set_error("Failed to allocate pool");
    return NULL;
  }

  p->pool = SDL_GPCreatePool(number_of_slots);
  if (!p->pool) {
    bxr_set_error("Failed to create pool with %zu slots (error: %s)",
                  number_of_slots,
                  SDL_GPGetErrorMessage(SDL_GPGetLastError()));
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
bxr_pool_acquire_slot(bxr_pool_t *pool)
{
  return SDL_GPAcquirePoolSlot(pool->pool);
}

void
bxr_pool_release_slot(bxr_pool_t *pool, int slot_index)
{
  SDL_GPReleasePoolSlot(pool->pool, slot_index);
}

Uint32
bxr_pool_generate_id(bxr_pool_t *pool, int slot_index)
{
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
