#include "bxr_resource.h"
#include "bxr_mem.h"

bxr_resource_t *
bxr_resource_make(size_t number_of_ids)
{
  bxr_resource_t *resource = NULL;
  BXR_NEW(resource);

  resource->size
      = number_of_ids + 1; // since the 0 index is used for invalid id
  resource->queue_top = 0;

  BXR_CALLOC(resource->counters, resource->size, sizeof(Uint32));

  // Since id 0 is reserved for invalid id, we only need number_of_ids slots
  BXR_CALLOC(resource->free_queue, number_of_ids, sizeof(int));

  for (int i = resource->size - 1; i >= 1; --i) {
    resource->free_queue[resource->queue_top++] = i;
    resource->counters[i]                       = 0;
  }

  return resource;
}

void
bxr_resource_destroy(bxr_resource_t *resource)
{
  BXR_FREE(resource->counters);
  BXR_FREE(resource->free_queue);
  BXR_FREE(resource);
}

int
bxr_resource_acquire_slot(bxr_resource_t *resource)
{
  SDL_assert(resource);
  SDL_assert(resource->free_queue);

  if (resource->queue_top > 0) {
    int slot = resource->free_queue[--resource->queue_top];
    return slot;
  } else {
    return _BXR_RESOURCE_INVALID_SLOT; // No more slots available
  }
}

void
bxr_resource_release_slot(bxr_resource_t *resource, int slot)
{
  SDL_assert(slot > _BXR_RESOURCE_INVALID_SLOT && slot < (int)resource->size);
  SDL_assert(resource);
  SDL_assert(resource->free_queue);
  SDL_assert(resource->queue_top < (int)resource->size);

#ifdef BXR_DEBUG
  for (int i = 0; i < resource->queue_top; ++i) {
    SDL_assert(resource->free_queue[i] != slot);
  }
#endif
  resource->free_queue[resource->queue_top++] = slot;

  SDL_assert(resource->queue_top <= (int)resource->size);
}

Uint32
bxr_resource_gen_id(bxr_resource_t *resource, int slot_index)
{
  SDL_assert(slot_index > _BXR_RESOURCE_INVALID_SLOT
             && slot_index < (int)resource->size);
  SDL_assert(resource);
  SDL_assert(resource->counters);

  uint32_t counter = ++resource->counters[slot_index]; // increment generation

  Uint32 id = (counter << _BXR_RESOURCE_SLOT_SHIFT)
              | (slot_index & _BXR_RESOURCE_SLOT_MASK);

  return id;
}

int
bxr_resource_id_to_slot(Uint32 id)
{
  int slot_index = (int)(id & _BXR_RESOURCE_SLOT_MASK);
  return slot_index;
}
