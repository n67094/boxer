#include <SDL3/SDL.h>

#include "bxr_bitset.h"
#include "bxr_config.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_mem.h"
#include "bxr_sparse_set.h"

typedef Uint32 _bxr_ecs_id_t;

typedef struct _bxr_ecs_component_array_s
{
  size_t length;
  size_t size; // Size of each component in bytes.
  void *data;
} _bxr_ecs_component_array_t;

typedef struct _bxr_ecs_component_data_s
{
  bxr_ecs_make_component_cb make_cb;
  bxr_ecs_destroy_component_cb destroy_cb;
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
  bxr_ecs_logic_cb logic_cb;
  bxr_ecs_system_add_cb add_cb;
  bxr_ecs_system_remove_cb remove_cb;
  bxr_bitset_t *require_bits;
  bxr_bitset_t *exclude_bits;
  void *udata;
} _bxr_ecs_system_data_t;

/**
 * Ecs data structure.
 *
 * `max_entities`
 *
 * `max_components`
 *
 * `max_systems`
 *
 * `entity_free_stack`
 *
 * `entity_add_stack`
 *
 * `entity_remove_stack`
 *
 * `entity_destroy_stack`
 */
struct bxr_ecs_s
{
  size_t max_entities;
  size_t max_components;
  size_t max_systems;

  _bxr_ecs_id_t *entity_free_stack;
  size_t entity_free_current;
  _bxr_ecs_id_t *entity_add_stack;
  size_t entity_add_current;
  _bxr_ecs_id_t *entity_remove_stack;
  size_t entity_remove_current;
  _bxr_ecs_id_t *entity_destroy_stack;
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
  if (ecs->entity_free_current >= ecs->max_entities) {
    return;
  }

  ecs->entity_free_stack[ecs->entity_free_current++] = entity;
}

static inline _bxr_ecs_id_t
_bxr_ecs_pop_add_entity(bxr_ecs_t *ecs)
{
  return ecs->entity_add_stack[--ecs->entity_add_current];
}

static inline void
_bxr_ecs_push_add_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_add_current >= ecs->max_entities) {
    return;
  }

  ecs->entity_add_stack[ecs->entity_add_current++] = entity;
}

static inline _bxr_ecs_id_t
_bxr_ecs_pop_remove_entity(bxr_ecs_t *ecs)
{
  return ecs->entity_remove_stack[--ecs->entity_remove_current];
}

static inline void
_bxr_ecs_push_remove_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_remove_current >= ecs->max_entities) {
    return;
  }

  ecs->entity_remove_stack[ecs->entity_remove_current++] = entity;
}

static inline _bxr_ecs_id_t
_bxr_ecs_pop_destroy_entity(bxr_ecs_t *ecs)
{
  return ecs->entity_destroy_stack[--ecs->entity_destroy_current];
}

static inline void
_bxr_ecs_push_destroy_entity(bxr_ecs_t *ecs, _bxr_ecs_id_t entity)
{
  if (ecs->entity_destroy_current >= ecs->max_entities) {
    return;
  }

  ecs->entity_destroy_stack[ecs->entity_destroy_current++] = entity;
}

bxr_ecs_t *
bxr_ecs_make(const bxr_ecs_desc_t *desc)
{
  SDL_assert(desc);

  bxr_ecs_t *ecs = NULL;
  BXR_NEW(ecs);
  if (ecs == NULL) {
    // TODO handle error
    return NULL;
  }

  BXR_MEMSET(ecs, 0, sizeof(bxr_ecs_t));

  // Set max entities, components, and systems from description or use defaults
  // if not provided
  ecs->max_entities = BXR_DEFAULT(desc->max_entities, BXR_ECS_DEFAULT_ENTITY);
  ecs->max_components
      = BXR_DEFAULT(desc->max_components, BXR_ECS_DEFAULT_COMPONENT);
  ecs->max_systems = BXR_DEFAULT(desc->max_systems, BXR_ECS_DEFAULT_SYSTEM);

  // Allocate entity ID stacks and initialize them to zero (no entities created)
  BXR_CALLOC(ecs->entity_free_stack, ecs->max_entities, sizeof(_bxr_ecs_id_t));
  ecs->entity_free_current = 0;
  BXR_CALLOC(ecs->entity_add_stack, ecs->max_entities, sizeof(_bxr_ecs_id_t));
  ecs->entity_add_current = 0;
  BXR_CALLOC(
      ecs->entity_remove_stack, ecs->max_entities, sizeof(_bxr_ecs_id_t));
  ecs->entity_remove_current = 0;
  BXR_CALLOC(
      ecs->entity_destroy_stack, ecs->max_entities, sizeof(_bxr_ecs_id_t));
  ecs->entity_destroy_current = 0;

  ecs->next_entity_id = 0;

  // Allocate component data array and initialize it to zero (no components
  // defined)
  BXR_CALLOC(ecs->component_arrays,
             ecs->max_components,
             sizeof(_bxr_ecs_component_array_t));
  BXR_MEMSET(ecs->component_arrays,
             0,
             ecs->max_components * sizeof(_bxr_ecs_component_array_t));

  // Allocate component data array and initialize it to zero (components with no
  // make/destroy callbacks)
  BXR_CALLOC(ecs->component_data,
             ecs->max_components,
             sizeof(_bxr_ecs_component_data_t));
  BXR_MEMSET(ecs->component_data,
             0,
             ecs->max_components * sizeof(_bxr_ecs_component_data_t));
  ecs->component_count = 0;

  // Allocate entity data array and initialize it to zero (inactive entities
  // with no components)
  BXR_CALLOC(
      ecs->entity_data, ecs->max_entities, sizeof(_bxr_ecs_entity_data_t));
  BXR_MEMSET(
      ecs->entity_data, 0, ecs->max_entities * sizeof(_bxr_ecs_entity_data_t));
  ecs->entity_data_count = 0;

  // Allocate system data array and initialize it to zero (inactive systems with
  // no entities)
  BXR_CALLOC(
      ecs->system_data, ecs->max_systems, sizeof(_bxr_ecs_system_data_t));
  BXR_MEMSET(
      ecs->system_data, 0, ecs->max_systems * sizeof(_bxr_ecs_system_data_t));
  ecs->system_count = 0;

  ecs->active_system = -1;

  return ecs;
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
    }
    if (system_data->exclude_bits) {
      bxr_bitset_destroy(system_data->exclude_bits);
    }
  }
  BXR_FREE(ecs->system_data);

  // Free entity data
  for (size_t i = 0; i < ecs->entity_data_count; i++) {
    _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[i];
    if (entity_data->component_bits) {
      bxr_bitset_destroy(entity_data->component_bits);
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

  // Free entity ID stacks
  BXR_FREE(ecs->entity_free_stack);
  BXR_FREE(ecs->entity_add_stack);
  BXR_FREE(ecs->entity_remove_stack);
  BXR_FREE(ecs->entity_destroy_stack);

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

  BXR_MEMSET(
      ecs->entity_data, 0, ecs->max_entities * sizeof(_bxr_ecs_entity_data_t));

  ecs->next_entity_id = 0;

  for (_bxr_ecs_id_t i = 0; i < ecs->system_count; i++) {
    bxr_sparse_set_clear(ecs->system_data[i].entity_ids);
  }
}

bxr_ecs_component_t
bxr_ecs_define_component(bxr_ecs_t *ecs,
                         bxr_ecs_make_component_cb make_cb,
                         bxr_ecs_destroy_component_cb destroy_cb)
{
  SDL_assert(ecs);
  SDL_assert(ecs->component_count < ecs->max_components);

  bxr_ecs_component_t id = ecs->component_count;

  _bxr_ecs_component_data_t *component_data = &ecs->component_data[id];

  component_data->make_cb    = make_cb;
  component_data->destroy_cb = destroy_cb;

  ecs->component_count++;

  return id;
}

bxr_ecs_system_t
bxr_ecs_define_system(bxr_ecs_t *ecs,
                      bxr_ecs_mask_t mask,
                      bxr_ecs_logic_cb logic_cb,
                      bxr_ecs_system_add_cb add_cb,
                      bxr_ecs_system_remove_cb remove_cb,
                      void *udata)
{
  SDL_assert(ecs);
  SDL_assert(ecs->system_count < ecs->max_systems);

  bxr_ecs_system_t id = ecs->system_count;

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[id];

  system_data->active       = true;
  system_data->mask         = mask;
  system_data->entity_ids   = bxr_sparse_set_make(ecs->max_entities);
  system_data->logic_cb     = logic_cb;
  system_data->add_cb       = add_cb;
  system_data->remove_cb    = remove_cb;
  system_data->require_bits = bxr_bitset_make(ecs->max_components);
  system_data->exclude_bits = bxr_bitset_make(ecs->max_components);
  system_data->udata        = udata;

  ecs->system_count++;

  return id;
}

void
bxr_ecs_require_component(bxr_ecs_t *ecs,
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
bxr_ecs_exclude_component(bxr_ecs_t *ecs,
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
bxr_ecs_enable_system(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];
  system_data->active                 = true;
}

void
bxr_ecs_disable_system(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  _bxr_ecs_system_data_t *system_data = &ecs->system_data[system];
  system_data->active                 = false;
}

void
bxr_ecs_set_system_callback(bxr_ecs_t *ecs,
                            bxr_ecs_system_t system,
                            bxr_ecs_logic_cb logic_cb,
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
bxr_ecs_set_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system, void *udata)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  ecs->system_data[system].udata = udata;
}

void *
bxr_ecs_get_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return ecs->system_data[system].udata;
}

void
bxr_ecs_set_system_mask(bxr_ecs_t *ecs,
                        bxr_ecs_system_t system,
                        bxr_ecs_mask_t mask)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  ecs->system_data[system].mask = mask;
}

bxr_ecs_mask_t
bxr_ecs_get_system_mask(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return ecs->system_data[system].mask;
}

size_t
bxr_ecs_get_system_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  SDL_assert(ecs);
  SDL_assert(system < ecs->max_systems);
  SDL_assert(system < ecs->system_count);

  return bxr_sparse_set_count(ecs->system_data[system].entity_ids);
}

bxr_ecs_entity_t
bxr_ecs_make_entity(bxr_ecs_t *ecs)
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
      BXR_MEMSET(ecs->entity_data + old_count,
                 0,
                 (new_count - old_count) * sizeof(_bxr_ecs_entity_data_t));

      ecs->entity_data_count = new_count;
    }
  }

  ecs->entity_data[entity_id].component_bits
      = bxr_bitset_make(ecs->max_components);
  ecs->entity_data[entity_id].active = true;
  ecs->entity_data[entity_id].ready  = true;

  return (bxr_ecs_entity_t)entity_id;
}

bool
bxr_ecs_is_entity_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  SDL_assert(ecs);
  SDL_assert(entity < ecs->max_entities);

  return ecs->entity_data[entity].ready;
}

bool
bxr_ecs_has_component(const bxr_ecs_t *ecs,
                      bxr_ecs_entity_t entity,
                      bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(entity < ecs->max_entities);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  return bxr_bitset_test(entity_data->component_bits, component);
}

void *
bxr_ecs_add_component(bxr_ecs_t *ecs,
                      bxr_ecs_entity_t entity,
                      bxr_ecs_component_t component,
                      void *args)
{
  SDL_assert(ecs);
  SDL_assert(entity < ecs->max_entities);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  // Get the entity
  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  // Get the component data and it's array of component
  _bxr_ecs_component_data_t *component_data = &ecs->component_data[component];
  _bxr_ecs_component_array_t *component_array
      = &ecs->component_arrays[component];

  // Grow component_array if needed
  if (entity >= component_array->length) {
    do {
      component_array->length = component_array->length * 2;
    } while (entity >= component_array->length);
    BXR_RESIZE(component_array,
               component_array->length * component_array->size);
  }

  // Get a pointer to the component's data and set it to zero
  void *component_ptr = bxr_ecs_get_component(ecs, entity, component);
  BXR_MEMSET(component_ptr, 0, component_array->size);

  if (component_data->make_cb)
    component_data->make_cb(ecs, entity, component_ptr, args);

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
    if (!bxr_bitset_true(exclude_overlap) && bxr_bitset_true(require_overlap)) {
      if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
        if (system_data->add_cb)
          system_data->add_cb(ecs, entity, system_data->udata);
      } else {
        // FREE bitset
        // TODO error
      }
    }
    // Optimization: If excluded, check if it was in the system and remove it
    else if (bxr_bitset_true(exclude_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      } else {
        // FREE bitset
        // TODO error
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

    if (!bxr_bitset_true(exclude_overlap) && bxr_bitset_true(require_overlap)) {
      size_t count    = bxr_sparse_set_capacity(set);
      size_t capacity = bxr_sparse_set_count(set);

      // Add the entity to the system directly
      if (count < capacity) {
        if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
          if (system_data->add_cb)
            system_data->add_cb(ecs, entity, system_data->udata);
        } else {
          // FREE bitset
          // TODO error
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
      // Add the entity using the derefered stack
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
bxr_ecs_get_component(const bxr_ecs_t *ecs,
                      bxr_ecs_entity_t entity,
                      bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(entity < ecs->max_entities);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  _bxr_ecs_component_array_t *component_array
      = &ecs->component_arrays[component];

  return (Uint8 *)component_array->data + (component_array->size * entity);
}

void
bxr_ecs_remove_component(bxr_ecs_t *ecs,
                         bxr_ecs_entity_t entity,
                         bxr_ecs_component_t component)
{
  SDL_assert(ecs);
  SDL_assert(entity < ecs->max_entities);
  SDL_assert(component < ecs->max_components);
  SDL_assert(component < ecs->component_count);

  _bxr_ecs_entity_data_t *entity_data = &ecs->entity_data[entity];

  bxr_bitset_t *component_bits = bxr_bitset_make(ecs->max_components);
  bxr_bitset_set(component_bits, component);

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

    if (!bxr_bitset_true(exclude_overlap) && bxr_bitset_true(require_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      } else {
        // FREE bitset
        // TODO error
      }
    }
    // Minor optimization that take advantage of this loop, check if the system
    // excludes components
    else {
      if (!bxr_bitset_is_zero(system_data->exclude_bits)) {
        // Remove the entity from the sparse set if its components no longer
        // match

        bxr_bitset_destroy(exclude_overlap);
        bxr_bitset_destroy(require_overlap);
      }
    }
  }

  bxr_bitset_destroy(component_bits);
}

void
bxr_ecs_destroy_component(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  //
}

int
bxr_ecs_run_system(bxr_ecs_t *ecs, bxr_ecs_system_t system, bxr_ecs_mask_t mask)
{
  //
}

int
ecs_run_systems(bxr_ecs_t *ecs, bxr_ecs_mask_t mask)
{
  //
}
