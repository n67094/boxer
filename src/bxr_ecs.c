#include <SDL3/SDL.h>

#include "bxr_bitset.h"
#include "bxr_config.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_error.h"
#include "bxr_mem.h"
#include "bxr_sparse_set.h"

typedef size_t _bxr_ecs_id_t;

typedef struct _bxr_ecs_component_array_s
{
  size_t capacity;
  size_t size; // Size of each component in bytes.
  void *data;
} _bxr_ecs_component_array_t;

typedef struct _bxr_ecs_component_data_s
{
  bxr_ecs_component_create_cb create_cb;
  bxr_ecs_component_destroy_cb destroy_cb;
} _bxr_ecs_component_data_t;

typedef struct _bxr_ecs_entity_data_s
{
  bxr_bitset_t *component_bits;
  bool active;
  bool ready;
} _bxr_ecs_entity_data_t;

typedef struct _bxr_ecs_system_data_s
{
  bool active;
  bxr_sparse_set_t *entity_ids;
  bxr_ecs_mask_t mask;
  bxr_ecs_system_logic_cb logic_cb;
  bxr_ecs_system_add_cb add_cb;
  bxr_ecs_system_remove_cb remove_cb;
  bxr_bitset_t *require_bits;
  bxr_bitset_t *exclude_bits;
  void *udata;
} _bxr_ecs_system_data_t;

struct bxr_ecs_s
{
  size_t max_components;
  size_t max_systems;

  _bxr_ecs_id_t *entity_free_stack;
  size_t entity_free_current;
  _bxr_ecs_id_t *entity_add_queue;
  size_t entity_add_current;
  _bxr_ecs_id_t *entity_remove_queue;
  size_t entity_remove_current;
  _bxr_ecs_id_t *entity_destroy_queue;
  size_t entity_destroy_current;

  size_t next_entity_id;

  _bxr_ecs_component_array_t *component_arrays;
  _bxr_ecs_component_data_t *component_data;
  size_t component_count;

  _bxr_ecs_entity_data_t *entity_data;
  size_t entity_data_count;

  _bxr_ecs_system_data_t *system_data;
  size_t system_count;

  int active_system;
};

static inline _bxr_ecs_id_t
_bxr_ecs_pop_free_entity(bxr_ecs_t *ecs)
{
  return ecs->entity_free_stack[--ecs->entity_free_current];
}

static inline void
_bxr_ecs_push_free_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_free_current >= ecs->entity_data_count) {
    return;
  }

  ecs->entity_free_stack[ecs->entity_free_current++] = entity;
}

static inline void
_bxr_ecs_push_add_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_add_current >= ecs->entity_data_count) {
    return;
  }

  ecs->entity_add_queue[ecs->entity_add_current++] = entity;
}

static void
_bxr_ecs_flush_add_entity(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_add_current; ++i) {
    _bxr_ecs_id_t entity = ecs->entity_add_queue[i];
    bxr_sparse_set_insert(ecs->system_data[system].entity_ids, entity);
  }

  ecs->entity_add_current = 0;
}

static inline void
_bxr_ecs_push_remove_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_remove_current >= ecs->entity_data_count) {
    return;
  }

  ecs->entity_remove_queue[ecs->entity_remove_current++] = entity;
}

static void
_bxr_ecs_flush_remove_entity(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_remove_current; ++i) {
    _bxr_ecs_id_t entity = ecs->entity_remove_queue[i];
    bxr_sparse_set_remove(ecs->system_data[system].entity_ids, entity);
  }

  ecs->entity_remove_current = 0;
}

static inline void
_bxr_ecs_push_destroy_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_destroy_current >= ecs->entity_data_count) {
    return;
  }

  ecs->entity_destroy_queue[ecs->entity_destroy_current++] = entity;
}

static void
_bxr_ecs_flush_destroy_entity(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_destroy_current; ++i) {
    _bxr_ecs_id_t entity = ecs->entity_destroy_queue[i];

    SDL_assert(ecs->entity_data[entity].active);
    bxr_sparse_set_remove(ecs->system_data[system].entity_ids, entity);

    // Push the entity ID to the free stack for reuse
    _bxr_ecs_push_free_entity(ecs, entity);

    BXR_MEMSET(&ecs->entity_data[entity], 0, sizeof(_bxr_ecs_entity_data_t));
  }

  ecs->entity_destroy_current = 0;
}

static void
_bxr_ecs_destruct(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  SDL_Log("Entity id: %zu active: %d, ready: %d ptr: %p",
          entity,
          entity_data->active,
          entity_data->ready,
          entity_data->component_bits);

  // Call component destroy callbacks
  for (bxr_ecs_component_t component = 0; component < ecs->component_count;
       component++) {
    if (bxr_bitset_test(entity_data->component_bits, component)) {
      _bxr_ecs_component_data_t *component_data
          = &ecs->component_data[component];
      if (component_data->destroy_cb) {
        void *component_ptr
            = bxr_ecs_entity_get_component(ecs, entity, component);
        component_data->destroy_cb(ecs, entity, component_ptr);
      }
    }
  }

  bxr_bitset_destroy(entity_data->component_bits);
  entity_data->component_bits = NULL;
}

bxr_ecs_t *
bxr_ecs_create(const bxr_ecs_desc_t *desc)
{
  SDL_assert(desc);

  bxr_ecs_t *ecs = NULL;
  BXR_NEW(ecs);
  if (!ecs) {
    goto free;
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
  }

  BXR_MEMSET(ecs, 0, sizeof(bxr_ecs_t));

  // Set max entities, components, and systems from description or use defaults
  // if not provided
  size_t max_entities = BXR_DEFAULT(desc->max_entities, BXR_ECS_DEFAULT_ENTITY);

  ecs->max_components
      = BXR_DEFAULT(desc->max_components, BXR_ECS_DEFAULT_COMPONENT);
  ecs->max_systems = BXR_DEFAULT(desc->max_systems, BXR_ECS_DEFAULT_SYSTEM);

  // Allocate entity ID stacks and initialize them to zero (no entities created)
  BXR_CALLOC(ecs->entity_free_stack, max_entities, sizeof(_bxr_ecs_id_t));
  if (!ecs->entity_free_stack) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  ecs->entity_free_current = 0;

  BXR_CALLOC(ecs->entity_add_queue, max_entities, sizeof(_bxr_ecs_id_t));
  if (!ecs->entity_add_queue) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  ecs->entity_add_current = 0;

  BXR_CALLOC(ecs->entity_remove_queue, max_entities, sizeof(_bxr_ecs_id_t));
  if (!ecs->entity_remove_queue) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  ecs->entity_remove_current = 0;

  BXR_CALLOC(ecs->entity_destroy_queue, max_entities, sizeof(_bxr_ecs_id_t));
  if (!ecs->entity_destroy_queue) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  ecs->entity_destroy_current = 0;

  ecs->next_entity_id = 0;

  // Allocate component data array and initialize it to zero (no components
  // defined)
  BXR_CALLOC(ecs->component_arrays,
             ecs->max_components,
             sizeof(_bxr_ecs_component_array_t));
  if (!ecs->component_arrays) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  BXR_MEMSET(ecs->component_arrays,
             0,
             ecs->max_components * sizeof(_bxr_ecs_component_array_t));

  // Allocate component data array and initialize it to zero (components with no
  // create/destroy callbacks)
  BXR_CALLOC(ecs->component_data,
             ecs->max_components,
             sizeof(_bxr_ecs_component_data_t));
  if (!ecs->component_data) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  BXR_MEMSET(ecs->component_data,
             0,
             ecs->max_components * sizeof(_bxr_ecs_component_data_t));
  ecs->component_count = 0;

  // Allocate entity data array and initialize it to zero (inactive entities
  // with no components)
  BXR_CALLOC(ecs->entity_data, max_entities, sizeof(_bxr_ecs_entity_data_t));
  if (!ecs->entity_data) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  BXR_MEMSET(
      ecs->entity_data, 0, max_entities * sizeof(_bxr_ecs_entity_data_t));
  ecs->entity_data_count = max_entities;

  // Allocate system data array and initialize it to zero (inactive systems with
  // no entities)
  BXR_CALLOC(
      ecs->system_data, ecs->max_systems, sizeof(_bxr_ecs_system_data_t));
  if (!ecs->system_data) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    goto free;
  }
  BXR_MEMSET(
      ecs->system_data, 0, ecs->max_systems * sizeof(_bxr_ecs_system_data_t));
  ecs->system_count = 0;

  ecs->active_system = -1;

  return ecs;

free:
  BXR_FREE(ecs);

  BXR_FREE(ecs->entity_free_stack);
  BXR_FREE(ecs->entity_add_queue);
  BXR_FREE(ecs->entity_remove_queue);
  BXR_FREE(ecs->entity_destroy_queue);

  BXR_FREE(ecs->component_arrays);
  BXR_FREE(ecs->component_data);

  BXR_FREE(ecs->entity_data);

  BXR_FREE(ecs->system_data);

  return NULL;
}

void
bxr_ecs_destroy(bxr_ecs_t *ecs)
{
  SDL_assert(ecs);

  // Free system data
  for (size_t i = 0; i < ecs->system_count; i++) {
    _bxr_ecs_system_data_t *system_data = &ecs->system_data[i];
    if (system_data->entity_ids) {
      bxr_sparse_set_destroy(system_data->entity_ids);
    }
    if (system_data->require_bits) {
      bxr_bitset_destroy(system_data->require_bits);
      system_data->require_bits = NULL;
    }
    if (system_data->exclude_bits) {
      bxr_bitset_destroy(system_data->exclude_bits);
      system_data->exclude_bits = NULL;
    }
  }
  BXR_FREE(ecs->system_data);

  // Free entity data
  for (size_t i = 0; i < ecs->entity_data_count; i++) {
    if (ecs->entity_data[i].active) {
      _bxr_ecs_destruct(ecs, i);
    }
  }

  BXR_FREE(ecs->entity_data);

  // Free component arrays and data
  for (size_t i = 0; i < ecs->component_count; i++) {
    _bxr_ecs_component_array_t *comp_array = &ecs->component_arrays[i];
    if (comp_array->data) {
      BXR_FREE(comp_array->data);
    }
  }
  BXR_FREE(ecs->component_arrays);
  BXR_FREE(ecs->component_data);

  // Free entity ID stack and queues
  BXR_FREE(ecs->entity_free_stack);
  BXR_FREE(ecs->entity_add_queue);
  BXR_FREE(ecs->entity_remove_queue);
  BXR_FREE(ecs->entity_destroy_queue);

  // Free ECS instance
  BXR_FREE(ecs);
}

void
bxr_ecs_reset(bxr_ecs_t *ecs)
{
  SDL_assert(ecs);

  // Reset entity ID stacks
  ecs->entity_free_current    = 0;
  ecs->entity_add_current     = 0;
  ecs->entity_remove_current  = 0;
  ecs->entity_destroy_current = 0;

  // Reset entity data
  for (size_t i = 0; i < ecs->entity_data_count; i++) {
    if (ecs->entity_data[i].active) {
      _bxr_ecs_destruct(ecs, i);
    }
  }

  // Reset entity data
  BXR_MEMSET(ecs->entity_data,
             0,
             ecs->entity_data_count * sizeof(_bxr_ecs_entity_data_t));

  // Reset entity ID generation
  ecs->next_entity_id = 0;

  for (_bxr_ecs_id_t i = 0; i < ecs->system_count; i++) {
    bxr_bitset_clear(ecs->system_data[i].require_bits);
    bxr_bitset_clear(ecs->system_data[i].exclude_bits);
    bxr_sparse_set_clear(ecs->system_data[i].entity_ids);
  }
}

/**
 * ECS Component API
 */

bxr_ecs_component_t
bxr_ecs_component_define(bxr_ecs_t *ecs,
                         size_t component_size,
                         bxr_ecs_component_create_cb create_cb,
                         bxr_ecs_component_destroy_cb destroy_cb)
{
  SDL_assert(ecs);
  SDL_assert(ecs->component_count < ecs->max_components);

  bxr_ecs_component_t id = ecs->component_count;

  // Allocate component array for the new component type and initialize it to
  // zero
  _bxr_ecs_component_array_t *component_array = &ecs->component_arrays[id];
  component_array->size                       = component_size;
  component_array->capacity
      = ecs->entity_data_count; // Start with capacity equal to max entities
  BXR_CALLOC(
      component_array->data, component_array->capacity, component_array->size);

  // Set component data for the new component type
  _bxr_ecs_component_data_t *component_data = &ecs->component_data[id];
  component_data->create_cb                 = create_cb;
  component_data->destroy_cb                = destroy_cb;

  ecs->component_count++;

  return id;
}

/**
 * ECS Entity API
 */

bxr_ecs_entity_t
bxr_ecs_entity_create(bxr_ecs_t *ecs)
{
  SDL_assert(ecs);

  _bxr_ecs_id_t entity_id = 0;

  // Try to reuse an entity ID from the free stack
  if (ecs->entity_free_current > 0) {
    entity_id = _bxr_ecs_pop_free_entity(ecs);
  }
  // If no free entity IDs, create a new one
  else {
    entity_id = ecs->next_entity_id++;

    // Grow the entity data array if necessary
    if (entity_id >= ecs->entity_data_count) {
      size_t old_count = ecs->entity_data_count;
      size_t new_count = old_count * 2;

      BXR_RESIZE(ecs->entity_data, new_count * sizeof(_bxr_ecs_entity_data_t));
      if (!ecs->entity_data) {
        bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
        return BXR_ECS_INVALID_ENTITY;
      }
      BXR_MEMSET(&ecs->entity_data[old_count],
                 0,
                 (new_count - old_count) * sizeof(_bxr_ecs_entity_data_t));

      ecs->entity_data_count = new_count;
    }
  }

  ecs->entity_data[entity_id].component_bits
      = bxr_bitset_create(ecs->max_components);
  ecs->entity_data[entity_id].active = true;
  ecs->entity_data[entity_id].ready  = true;

  return (bxr_ecs_entity_t)entity_id;
}

bool
bxr_ecs_entity_is_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  SDL_assert(ecs);

  return ecs->entity_data[entity].ready;
}

bool
bxr_ecs_entity_has_component(const bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  SDL_assert(ecs->entity_data[entity].ready);

  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  return bxr_bitset_test(entity_data->component_bits, component);
}

void *
bxr_ecs_entity_add_component(bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component,
                             void *args)
{
  SDL_assert(ecs);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);
  SDL_assert(ecs->entity_data[entity].ready);

  // Get the entity
  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  // Get the component data and it's array of component
  _bxr_ecs_component_data_t *component_data = &ecs->component_data[component];
  _bxr_ecs_component_array_t *component_array
      = &ecs->component_arrays[component];

  // Grow component_array if needed
  if (entity >= component_array->capacity) {
    do {
      component_array->capacity *= 2;
    } while (entity >= component_array->capacity);

    BXR_RESIZE(component_array->data,
               component_array->capacity * component_array->size);
    if (!component_array->data) {
      bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
      return NULL;
    }
  }

  // Get a pointer to the component's data and set it to zero
  void *component_ptr = bxr_ecs_entity_get_component(ecs, entity, component);
  BXR_MEMSET(component_ptr, 0, component_array->size);

  if (component_data->create_cb)
    component_data->create_cb(ecs, entity, component_ptr, args);

  // Set the entity's component bit
  bxr_bitset_set(entity_data->component_bits, component);

  // Add or remove entity from system (non running ones)
  for (_bxr_ecs_id_t system_id = 0; system_id < ecs->system_count;
       system_id++) {
    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    _bxr_ecs_system_data_t *system_data = &ecs->system_data[system_id];

    bxr_bitset_t *exclude_overlap = bxr_bitset_and(entity_data->component_bits,
                                                   system_data->exclude_bits);

    bxr_bitset_t *require_overlap = bxr_bitset_and(entity_data->component_bits,
                                                   system_data->require_bits);

    // If not excluded and required add it
    if (!bxr_bitset_true(exclude_overlap)
        && bxr_bitset_equal(require_overlap, system_data->require_bits)) {
      if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
        if (system_data->add_cb)
          system_data->add_cb(ecs, entity, system_data->udata);
      }
    }
    // If the added component causes the entity to be excluded, check if it was
    // in the system and remove it
    else if (bxr_bitset_true(exclude_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
    }

    bxr_bitset_destroy(exclude_overlap);
    bxr_bitset_destroy(require_overlap);
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    _bxr_ecs_system_data_t *system_data = &ecs->system_data[ecs->active_system];

    bxr_bitset_t *exclude_overlap = bxr_bitset_and(entity_data->component_bits,
                                                   system_data->exclude_bits);

    bxr_bitset_t *require_overlap = bxr_bitset_and(entity_data->component_bits,
                                                   system_data->require_bits);

    bxr_sparse_set_t *set = system_data->entity_ids;

    if (!bxr_bitset_true(exclude_overlap)
        && bxr_bitset_equal(require_overlap, system_data->require_bits)) {
      size_t count    = bxr_sparse_set_capacity(set);
      size_t capacity = bxr_sparse_set_count(set);

      // Add the entity to the system directly
      if (count < capacity) {
        if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
          if (system_data->add_cb)
            system_data->add_cb(ecs, entity, system_data->udata);
        }
      }
      // Add the entity using the derefered stack
      else {
        if (!bxr_sparse_set_contains(set, entity, NULL)) {
          if (system_data->add_cb)
            system_data->add_cb(ecs, entity, system_data->udata);

          _bxr_ecs_push_add_entity(ecs, entity);
        }
      }
    }
    // Optimization: If excluded, check if it was in the system and remove it
    else if (bxr_bitset_true(exclude_overlap)) {
      // If the added component causes the entity to be excluded, check if it
      // was in the system and remove it
      if (bxr_sparse_set_contains(set, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        _bxr_ecs_push_remove_entity(ecs, entity);
      }
    }

    bxr_bitset_destroy(exclude_overlap);
    bxr_bitset_destroy(require_overlap);
  }

  return component_ptr;
}

void *
bxr_ecs_entity_get_component(const bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);
  SDL_assert(ecs->entity_data[entity].ready);

  _bxr_ecs_component_array_t *component_array
      = &ecs->component_arrays[component];

  return (Uint8 *)component_array->data + (component_array->size * entity);
}

void
bxr_ecs_entity_remove_component(bxr_ecs_t *ecs,
                                bxr_ecs_entity_t entity,
                                bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);
  SDL_assert(ecs->entity_data[entity].ready);

  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  bxr_bitset_t *component_bits = bxr_bitset_create(ecs->max_components);
  bxr_bitset_set(component_bits, component);

  bxr_bitset_t *next_component_bits
      = bxr_bitset_copy(entity_data->component_bits);
  bxr_bitset_unset(next_component_bits, component);

  for (_bxr_ecs_id_t system_id = 0; system_id < ecs->system_count;
       system_id++) {

    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    _bxr_ecs_system_data_t *system_data = &ecs->system_data[system_id];

    bxr_bitset_t *exclude_overlap
        = bxr_bitset_and(component_bits, system_data->exclude_bits);
    bxr_bitset_t *require_overlap
        = bxr_bitset_and(component_bits, system_data->require_bits);

    // If not excluded and required remove it
    if (!bxr_bitset_true(exclude_overlap) && bxr_bitset_true(require_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
      // If the entity with the removed component is equal the the system's
      // required components, add it to the system
    } else if (bxr_bitset_equal(next_component_bits,
                                system_data->require_bits)) {
      if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
        if (system_data->add_cb)
          system_data->add_cb(ecs, entity, system_data->udata);
      }
    }

    bxr_bitset_destroy(exclude_overlap);
    bxr_bitset_destroy(require_overlap);
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    _bxr_ecs_system_data_t *system_data = &ecs->system_data[ecs->active_system];

    bxr_bitset_t *exclude_overlap
        = bxr_bitset_and(component_bits, system_data->exclude_bits);
    bxr_bitset_t *require_overlap
        = bxr_bitset_and(component_bits, system_data->require_bits);

    // If not excluded and required remove it
    if (!bxr_bitset_true(exclude_overlap) && bxr_bitset_true(require_overlap)) {
      if (bxr_sparse_set_contains(system_data->entity_ids, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        _bxr_ecs_push_remove_entity(ecs, entity);
      }
    }
    // If the entity with the removed component is equal the the system's
    // required components, add it to the system
    else if (bxr_bitset_equal(next_component_bits, system_data->require_bits)) {
      bxr_sparse_set_t *set = system_data->entity_ids;

      size_t count    = bxr_sparse_set_capacity(set);
      size_t capacity = bxr_sparse_set_count(set);

      if (count < capacity) {
        if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
          if (system_data->add_cb)
            system_data->add_cb(ecs, entity, system_data->udata);
        }
      } else {
        if (!bxr_sparse_set_contains(set, entity, NULL)) {
          if (system_data->add_cb)
            system_data->add_cb(ecs, entity, system_data->udata);

          _bxr_ecs_push_add_entity(ecs, entity);
        }
      }

      bxr_bitset_destroy(exclude_overlap);
      bxr_bitset_destroy(require_overlap);
    }
  }

  bxr_bitset_destroy(component_bits);
  bxr_bitset_destroy(next_component_bits);

  _bxr_ecs_component_data_t *component_data = &ecs->component_data[component];

  if (component_data->destroy_cb) {
    void *component_ptr = bxr_ecs_entity_get_component(ecs, entity, component);
    component_data->destroy_cb(ecs, entity, component_ptr);
  }

  // Unset the entity's component bit
  bxr_bitset_unset(entity_data->component_bits, component);
}

void
bxr_ecs_entity_destroy(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  SDL_assert(ecs);
  SDL_assert(ecs->entity_data[entity].active);

  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  // Remove entity from systems
  for (_bxr_ecs_id_t system_id = 0; system_id < ecs->system_count;
       system_id++) {

    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    _bxr_ecs_system_data_t *system_data = &ecs->system_data[system_id];

    bxr_bitset_t *exclude_overlap = bxr_bitset_and(
        entity_data->component_bits, ecs->system_data[system_id].exclude_bits);
    bxr_bitset_t *require_overlap = bxr_bitset_and(
        entity_data->component_bits, ecs->system_data[system_id].require_bits);

    if (!bxr_bitset_true(exclude_overlap)
        && bxr_bitset_equal(require_overlap, system_data->require_bits)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
    }

    bxr_bitset_destroy(exclude_overlap);
    bxr_bitset_destroy(require_overlap);
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    _bxr_ecs_system_data_t *system_data = &ecs->system_data[ecs->active_system];

    bxr_bitset_t *exclude_overlap = bxr_bitset_and(
        entity_data->component_bits, ecs->system_data->exclude_bits);
    bxr_bitset_t *require_overlap = bxr_bitset_and(
        entity_data->component_bits, ecs->system_data->require_bits);

    if (!bxr_bitset_true(exclude_overlap)
        && bxr_bitset_equal(require_overlap, system_data->require_bits)) {

      if (bxr_sparse_set_contains(system_data->entity_ids, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        _bxr_ecs_push_destroy_entity(ecs, entity);

        entity_data->ready  = false;
        entity_data->active = true;
      }
    }

    bxr_bitset_destroy(exclude_overlap);
    bxr_bitset_destroy(require_overlap);
  }

  _bxr_ecs_destruct(ecs, entity);

  // If no system is running, clean up the entity immediately and push it to the
  // free stack for re-use
  if (ecs->active_system < 0) {
    _bxr_ecs_push_free_entity(ecs, entity);
    BXR_MEMSET(entity_data, 0, sizeof(_bxr_ecs_entity_data_t));
  }
}

/**
 * ECS System API
 */

bxr_ecs_system_t
bxr_ecs_system_define(bxr_ecs_t *ecs,
                      bxr_ecs_mask_t mask,
                      bxr_ecs_system_logic_cb logic_cb,
                      bxr_ecs_system_add_cb add_cb,
                      bxr_ecs_system_remove_cb remove_cb,
                      void *udata)
{
  SDL_assert(ecs);
  SDL_assert(ecs->system_count < ecs->max_systems);

  bxr_ecs_system_t id = ecs->system_count;

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[id];

  system_data->active     = true;
  system_data->mask       = mask;
  system_data->entity_ids = bxr_sparse_set_create(
      ecs->entity_data_count); // Start with capacity equal to max entities
  system_data->logic_cb     = logic_cb;
  system_data->add_cb       = add_cb;
  system_data->remove_cb    = remove_cb;
  system_data->require_bits = bxr_bitset_create(ecs->max_components);
  system_data->exclude_bits = bxr_bitset_create(ecs->max_components);
  system_data->udata        = udata;

  ecs->system_count++;

  return id;
}

void
bxr_ecs_system_require_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t system,
                                 bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];

  bxr_bitset_set(system_data->require_bits, component);
}

void
bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t system,
                                 bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];

  bxr_bitset_set(system_data->exclude_bits, component);
}

void
bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];
  system_data->active                 = true;
}

void
bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];
  system_data->active                 = false;
}

void
bxr_ecs_system_set_callback(bxr_ecs_t *ecs,
                            bxr_ecs_system_t system,
                            bxr_ecs_system_logic_cb logic_cb,
                            bxr_ecs_system_add_cb add_cb,
                            bxr_ecs_system_remove_cb remove_cb)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];

  system_data->logic_cb  = logic_cb;
  system_data->add_cb    = add_cb;
  system_data->remove_cb = remove_cb;
}

void
bxr_ecs_system_set_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system, void *udata)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  ecs->system_data[system].udata = udata;
}

void *
bxr_ecs_system_get_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return ecs->system_data[system].udata;
}

void
bxr_ecs_system_set_mask(bxr_ecs_t *ecs,
                        bxr_ecs_system_t system,
                        bxr_ecs_mask_t mask)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  ecs->system_data[system].mask = mask;
}

bxr_ecs_mask_t
bxr_ecs_system_get_mask(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return ecs->system_data[system].mask;
}

size_t
bxr_ecs_system_get_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return bxr_sparse_set_count(ecs->system_data[system].entity_ids);
}

int
bxr_ecs_system_run(bxr_ecs_t *ecs, bxr_ecs_system_t system, bxr_ecs_mask_t mask)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];
  if (!system_data->active)
    return 0;

  if (system_data->mask != 0 && !(system_data->mask & mask))
    return 0;

  ecs->active_system = system;

  size_t *entities      = bxr_sparse_set_get_dense(system_data->entity_ids);
  size_t entities_count = bxr_sparse_set_count(system_data->entity_ids);

  int retval = system_data->logic_cb(
      ecs, entities, entities_count, system_data->udata);

  _bxr_ecs_flush_add_entity(ecs, system);
  _bxr_ecs_flush_remove_entity(ecs, system);
  _bxr_ecs_flush_destroy_entity(ecs, system);

  ecs->active_system = -1;

  return retval;
}

int
bxr_ecs_systems_run(bxr_ecs_t *ecs, bxr_ecs_mask_t mask)
{
  SDL_assert(ecs);

  for (_bxr_ecs_id_t system_id = 0; system_id < ecs->system_count;
       system_id++) {

    int retval = bxr_ecs_system_run(ecs, system_id, mask);

    if (retval != 0) {
      return retval;
    }
  }

  return 0;
}
