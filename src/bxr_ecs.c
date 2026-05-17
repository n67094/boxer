#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_config.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_log.h"
#include "bxr_mem.h"
#include "bxr_sparse_set.h"

/**
 * ECS bitset API
 */

#define BXR_ECS_BITSET_PITCH_SIZE_ (sizeof(size_t) * 8)
#define BXR_ECS_BITSET_SIZE_                                                   \
  (((BXR_ECS_MAX_COMPONENT - 1) / BXR_ECS_BITSET_PITCH_SIZE_) + 1)

typedef struct bxr_bitset_s_
{
  size_t bits[BXR_ECS_BITSET_SIZE_];
} bxr_ecs_bitset_t_;

static bool
bxr_ecs_bitset_true_(bxr_ecs_bitset_t_ *bitset)
{
  for (size_t i = 0; i < BXR_ECS_BITSET_SIZE_; i++) {
    if (bitset->bits[i]) {
      return true;
    }
  }

  return false;
}

static bool
bxr_ecs_bitset_test_(const bxr_ecs_bitset_t_ *bitset, size_t index)
{
  BXR_ASSERT(index < BXR_ECS_BITSET_PITCH_SIZE_ * BXR_ECS_BITSET_SIZE_);

  size_t word_index = index / BXR_ECS_BITSET_PITCH_SIZE_;
  size_t bit_index  = index % BXR_ECS_BITSET_PITCH_SIZE_;

  return (bitset->bits[word_index] & ((size_t)1 << bit_index)) != 0;
}

static void
bxr_ecs_bitset_set_(bxr_ecs_bitset_t_ *bitset, size_t index)
{
  BXR_ASSERT(index < BXR_ECS_BITSET_PITCH_SIZE_ * BXR_ECS_BITSET_SIZE_);

  size_t word_index = index / BXR_ECS_BITSET_PITCH_SIZE_;
  size_t bit_index  = index % BXR_ECS_BITSET_PITCH_SIZE_;

  bitset->bits[word_index] |= ((size_t)1 << bit_index);
}

static void
bxr_ecs_bitset_unset_(bxr_ecs_bitset_t_ *bitset, size_t index)
{
  BXR_ASSERT(index < BXR_ECS_BITSET_PITCH_SIZE_ * BXR_ECS_BITSET_SIZE_);

  size_t word_index = index / BXR_ECS_BITSET_PITCH_SIZE_;
  size_t bit_index  = index % BXR_ECS_BITSET_PITCH_SIZE_;

  bitset->bits[word_index] &= ~((size_t)1 << bit_index);
}

static bxr_ecs_bitset_t_
bxr_ecs_bitset_and_(const bxr_ecs_bitset_t_ *a, const bxr_ecs_bitset_t_ *b)
{
  bxr_ecs_bitset_t_ result = { 0 };

  for (size_t i = 0; i < BXR_ECS_BITSET_SIZE_; i++) {
    result.bits[i] = a->bits[i] & b->bits[i];
  }

  return result;
}

bxr_ecs_bitset_t_
bxr_ecs_bitset_or_(const bxr_ecs_bitset_t_ *a, const bxr_ecs_bitset_t_ *b)
{
  bxr_ecs_bitset_t_ result = { 0 };

  for (size_t i = 0; i < BXR_ECS_BITSET_SIZE_; i++) {
    result.bits[i] = a->bits[i] | b->bits[i];
  }

  return result;
}

bxr_ecs_bitset_t_
bxr_ecs_bitset_copy_(const bxr_ecs_bitset_t_ *src)
{
  bxr_ecs_bitset_t_ dest = { 0 };

  for (size_t i = 0; i < BXR_ECS_BITSET_SIZE_; i++) {
    dest.bits[i] = src->bits[i];
  }

  return dest;
}

bool
bxr_ecs_bitset_equal_(const bxr_ecs_bitset_t_ *a, const bxr_ecs_bitset_t_ *b)
{
  for (size_t i = 0; i < BXR_ECS_BITSET_SIZE_; i++) {
    if (a->bits[i] != b->bits[i]) {
      return false;
    }
  }

  return true;
}

/**
 * ECS API
 */

typedef size_t bxr_ecs_id_t_;

typedef struct bxr_ecs_component_array_s_
{
  size_t capacity;
  size_t size; // Size of each component in bytes.
  void *data;
} bxr_ecs_component_array_t_;

typedef struct bxr_ecs_component_data_s_
{
  bxr_ecs_component_create_cb create_cb;
  bxr_ecs_component_destroy_cb destroy_cb;
} bxr_ecs_component_data_t_;

typedef struct bxr_ecs_entity_data_s_
{
  bxr_ecs_bitset_t_ component_bits;
  bool active;
  bool ready;
} bxr_ecs_entity_data_t_;

typedef struct bxr_ecs_system_data_s_
{
  bool active;
  bxr_sparse_set_t *entity_ids;
  bxr_ecs_mask_t mask;
  bxr_ecs_system_logic_cb logic_cb;
  bxr_ecs_system_add_cb add_cb;
  bxr_ecs_system_remove_cb remove_cb;
  bxr_ecs_bitset_t_ require_bits;
  bxr_ecs_bitset_t_ exclude_bits;
  void *udata;
} bxr_ecs_system_data_t_;

struct bxr_ecs_s
{
  bxr_ecs_id_t_ *entity_free_stack;
  size_t entity_free_capacity;
  size_t entity_free_current;

  bxr_ecs_id_t_ *entity_add_queue;
  size_t entity_add_capacity;
  size_t entity_add_current;

  bxr_ecs_id_t_ *entity_remove_queue;
  size_t entity_remove_capacity;
  size_t entity_remove_current;

  bxr_ecs_id_t_ *entity_destroy_queue;
  size_t entity_destroy_capacity;
  size_t entity_destroy_current;

  size_t next_entity_id;

  bxr_ecs_component_array_t_ component_arrays[BXR_ECS_MAX_COMPONENT];
  bxr_ecs_component_data_t_ components[BXR_ECS_MAX_COMPONENT];
  size_t component_count;

  bxr_ecs_entity_data_t_ *entities;
  size_t entity_count;

  bxr_ecs_system_data_t_ systems[BXR_ECS_MAX_SYSTEM];
  size_t system_count;

  int active_system;
};

static inline bxr_ecs_id_t_
bxr_ecs_pop_free_entity_(bxr_ecs_t *ecs)
{
  return ecs->entity_free_stack[--ecs->entity_free_current];
}

static inline void
bxr_ecs_push_free_entity_(bxr_ecs_t *ecs, bxr_ecs_id_t_ entity)
{
  if (ecs->entity_free_current >= ecs->entity_count) {
    return;
  }

  if (ecs->entity_free_capacity < ecs->entity_count) {
    BXR_RESIZE(ecs->entity_free_stack,
               ecs->entity_count * sizeof(bxr_ecs_id_t_));
    if (!ecs->entity_free_stack) {
      return;
    }
  }

  ecs->entity_free_stack[ecs->entity_free_current++] = entity;
}

static inline void
bxr_ecs_push_add_entity_(bxr_ecs_t *ecs, bxr_ecs_id_t_ entity)
{
  if (ecs->entity_add_current >= ecs->entity_count) {
    return;
  }

  if (ecs->entity_add_capacity < ecs->entity_count) {
    BXR_RESIZE(ecs->entity_add_queue,
               ecs->entity_count * sizeof(bxr_ecs_id_t_));
    if (!ecs->entity_add_queue) {
      return;
    }
  }

  ecs->entity_add_queue[ecs->entity_add_current++] = entity;
}

static void
bxr_ecs_flush_add_entity_(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_add_current; ++i) {
    bxr_ecs_id_t_ entity = ecs->entity_add_queue[i];
    bxr_sparse_set_insert(ecs->systems[system].entity_ids, entity);
  }

  ecs->entity_add_current = 0;
}

static inline void
bxr_ecs_push_remove_entity_(bxr_ecs_t *ecs, bxr_ecs_id_t_ entity)
{
  if (ecs->entity_remove_current >= ecs->entity_count) {
    return;
  }

  if (ecs->entity_remove_capacity < ecs->entity_count) {
    BXR_RESIZE(ecs->entity_remove_queue,
               ecs->entity_count * sizeof(bxr_ecs_id_t_));
    if (!ecs->entity_remove_queue) {
      return;
    }
  }

  ecs->entity_remove_queue[ecs->entity_remove_current++] = entity;
}

static void
bxr_ecs_flush_remove_entity_(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_remove_current; ++i) {
    bxr_ecs_id_t_ entity = ecs->entity_remove_queue[i];
    bxr_sparse_set_remove(ecs->systems[system].entity_ids, entity);
  }

  ecs->entity_remove_current = 0;
}

static inline void
bxr_ecs_push_destroy_entity_(bxr_ecs_t *ecs, bxr_ecs_id_t_ entity)
{
  if (ecs->entity_destroy_current >= ecs->entity_count) {
    return;
  }

  if (ecs->entity_destroy_capacity < ecs->entity_count) {
    BXR_RESIZE(ecs->entity_destroy_queue,
               ecs->entity_count * sizeof(bxr_ecs_id_t_));
    if (!ecs->entity_destroy_queue) {
      return;
    }
  }

  ecs->entity_destroy_queue[ecs->entity_destroy_current++] = entity;
}

static void
bxr_ecs_flush_destroy_entity_(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  for (size_t i = 0; i < ecs->entity_destroy_current; ++i) {
    bxr_ecs_id_t_ entity = ecs->entity_destroy_queue[i];

    BXR_ASSERT(ecs->entities[entity].active);
    bxr_sparse_set_remove(ecs->systems[system].entity_ids, entity);

    // Push the entity ID to the free stack for reuse
    bxr_ecs_push_free_entity_(ecs, entity);

    BXR_MEMSET(&ecs->entities[entity], 0, sizeof(bxr_ecs_entity_data_t_));
  }

  ecs->entity_destroy_current = 0;
}

static void
bxr_ecs_destruct_(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  bxr_ecs_entity_data_t_ *entity_data = &ecs->entities[entity];

  // Call component destroy callbacks
  for (bxr_ecs_component_t component = 0; component < ecs->component_count;
       component++) {
    if (bxr_ecs_bitset_test_(&entity_data->component_bits, component)) {
      bxr_ecs_component_data_t_ *component_data = &ecs->components[component];
      if (component_data->destroy_cb) {
        void *component_ptr
            = bxr_ecs_entity_get_component(ecs, entity, component);
        component_data->destroy_cb(ecs, entity, component_ptr);
      }
    }
  }
}

bxr_ecs_t *
bxr_ecs_create(size_t entity_count)
{
  bxr_ecs_t *ecs = NULL;
  BXR_NEW(ecs);
  if (!ecs) {
    goto free;
  }
  BXR_MEMSET(ecs, 0, sizeof(bxr_ecs_t));

  // Set max entities, components, and systems from description or use defaults
  // if not provided
  entity_count = BXR_DEFAULT(entity_count, BXR_ECS_DEFAULT_ENTITY);

  // Allocate entity ID stacks and initialize them to zero (no entities created)
  BXR_CALLOC(ecs->entity_free_stack, entity_count, sizeof(bxr_ecs_id_t_));
  if (!ecs->entity_free_stack) {
    goto free;
  }
  ecs->entity_free_capacity = entity_count;
  ecs->entity_free_current  = 0;

  BXR_CALLOC(ecs->entity_add_queue, entity_count, sizeof(bxr_ecs_id_t_));
  if (!ecs->entity_add_queue) {
    goto free;
  }
  ecs->entity_add_capacity = entity_count;
  ecs->entity_add_current  = 0;

  BXR_CALLOC(ecs->entity_remove_queue, entity_count, sizeof(bxr_ecs_id_t_));
  if (!ecs->entity_remove_queue) {
    goto free;
  }
  ecs->entity_remove_capacity = entity_count;
  ecs->entity_remove_current  = 0;

  BXR_CALLOC(ecs->entity_destroy_queue, entity_count, sizeof(bxr_ecs_id_t_));
  if (!ecs->entity_destroy_queue) {
    goto free;
  }
  ecs->entity_destroy_capacity = entity_count;
  ecs->entity_destroy_current  = 0;

  // Allocate entity data array and initialize it to zero (inactive entities
  // with no components)
  BXR_CALLOC(ecs->entities, entity_count, sizeof(bxr_ecs_entity_data_t_));
  if (!ecs->entities) {
    goto free;
  }

  BXR_MEMSET(ecs->entities, 0, entity_count * sizeof(bxr_ecs_entity_data_t_));
  ecs->entity_count = entity_count;

  ecs->active_system  = -1;
  ecs->next_entity_id = 0;

  return ecs;

free:
  BXR_FREE(ecs->entity_free_stack);
  BXR_FREE(ecs->entity_add_queue);
  BXR_FREE(ecs->entity_remove_queue);
  BXR_FREE(ecs->entity_destroy_queue);

  BXR_FREE(ecs->entities);

  BXR_FREE(ecs);

  return NULL;
}

void
bxr_ecs_destroy(bxr_ecs_t *ecs)
{
  BXR_ASSERT(ecs);

  // Free system data
  for (size_t i = 0; i < ecs->system_count; i++) {
    bxr_ecs_system_data_t_ *system_data = &ecs->systems[i];
    if (system_data->entity_ids) {
      bxr_sparse_set_destroy(system_data->entity_ids);
    }
  }

  // Free entity data
  for (size_t i = 0; i < ecs->entity_count; i++) {
    if (ecs->entities[i].active) {
      bxr_ecs_destruct_(ecs, i);
    }
  }

  BXR_FREE(ecs->entities);

  // Free component arrays and data
  for (size_t i = 0; i < ecs->component_count; i++) {
    bxr_ecs_component_array_t_ *comp_array = &ecs->component_arrays[i];
    if (comp_array->data) {
      BXR_FREE(comp_array->data);
    }
  }

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
  BXR_ASSERT(ecs);

  // Reset entity ID stacks
  ecs->entity_free_current    = 0;
  ecs->entity_add_current     = 0;
  ecs->entity_remove_current  = 0;
  ecs->entity_destroy_current = 0;

  // Reset entity data
  for (size_t i = 0; i < ecs->entity_count; i++) {
    if (ecs->entities[i].active) {
      bxr_ecs_destruct_(ecs, i);
    }
  }

  // Reset entity data
  BXR_MEMSET(
      ecs->entities, 0, ecs->entity_count * sizeof(bxr_ecs_entity_data_t_));

  // Reset entity ID generation
  ecs->next_entity_id = 0;

  // Reset system data
  for (bxr_ecs_id_t_ i = 0; i < ecs->system_count; i++) {
    bxr_sparse_set_clear(ecs->systems[i].entity_ids);
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
  BXR_ASSERT(ecs);
  BXR_ASSERT(ecs->component_count < BXR_ECS_MAX_COMPONENT);

  bxr_ecs_component_t id = ecs->component_count;

  // Allocate component array for the new component type and initialize it to
  // zero
  bxr_ecs_component_array_t_ *component_array = &ecs->component_arrays[id];
  component_array->size                       = component_size;
  component_array->capacity
      = ecs->entity_count; // Start with capacity equal to max entities
  BXR_CALLOC(
      component_array->data, component_array->capacity, component_array->size);

  // Set component data for the new component type
  bxr_ecs_component_data_t_ *component_data = &ecs->components[id];
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
  BXR_ASSERT(ecs);

  bxr_ecs_id_t_ entity_id = 0;

  // Try to reuse an entity ID from the free stack
  if (ecs->entity_free_current > 0) {
    entity_id = bxr_ecs_pop_free_entity_(ecs);
  }
  // If no free entity IDs, create a new one
  else {
    entity_id = ecs->next_entity_id++;

    // Grow the entity data array if necessary
    if (entity_id >= ecs->entity_count) {
      size_t old_count = ecs->entity_count;
      size_t new_count = old_count * 2;

      BXR_RESIZE(ecs->entities, new_count * sizeof(bxr_ecs_entity_data_t_));
      if (!ecs->entities) {
        return BXR_ECS_INVALID_ENTITY;
      }
      BXR_MEMSET(&ecs->entities[old_count],
                 0,
                 (new_count - old_count) * sizeof(bxr_ecs_entity_data_t_));

      ecs->entity_count = new_count;
    }
  }

  ecs->entities[entity_id].component_bits = (bxr_ecs_bitset_t_){ 0 };
  ecs->entities[entity_id].active         = true;
  ecs->entities[entity_id].ready          = true;

  return (bxr_ecs_entity_t)entity_id;
}

bool
bxr_ecs_entity_is_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  BXR_ASSERT(ecs);

  return ecs->entities[entity].ready;
}

bool
bxr_ecs_entity_has_component(const bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);

  BXR_ASSERT(ecs->entities[entity].ready);

  bxr_ecs_entity_data_t_ *entity_data = &ecs->entities[entity];

  return bxr_ecs_bitset_test_(&entity_data->component_bits, component);
}

void *
bxr_ecs_entity_add_component(bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component,
                             void *args)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);
  BXR_ASSERT(ecs->entities[entity].ready);

  // Get the entity
  bxr_ecs_entity_data_t_ *entity_data = &ecs->entities[entity];

  // Get the component data and it's array of component
  bxr_ecs_component_data_t_ *component_data = &ecs->components[component];
  bxr_ecs_component_array_t_ *component_array
      = &ecs->component_arrays[component];

  // Grow component_array if needed
  if (entity >= component_array->capacity) {
    do {
      component_array->capacity *= 2;
    } while (entity >= component_array->capacity);

    BXR_RESIZE(component_array->data,
               component_array->capacity * component_array->size);
    if (!component_array->data) {
      return NULL;
    }
  }

  // Get a pointer to the component's data and set it to zero
  void *component_ptr = bxr_ecs_entity_get_component(ecs, entity, component);
  BXR_MEMSET(component_ptr, 0, component_array->size);

  if (component_data->create_cb)
    component_data->create_cb(ecs, entity, component_ptr, args);

  // Set the entity's component bit
  bxr_ecs_bitset_set_(&entity_data->component_bits, component);

  for (bxr_ecs_id_t_ system_id = 0; system_id < ecs->system_count;
       system_id++) {
    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    bxr_ecs_system_data_t_ *system_data = &ecs->systems[system_id];

    bxr_ecs_bitset_t_ exclude_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->exclude_bits);

    bxr_ecs_bitset_t_ require_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->require_bits);

    // If not excluded and required then the entity should be added to the
    // system
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_equal_(&require_overlap,
                                 &system_data->require_bits)) {
      if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
        if (system_data->add_cb)
          system_data->add_cb(ecs, entity, system_data->udata);
      }
    }
    // If the added component causes the entity to be excluded, check if it was
    // in the system and remove it
    else if (bxr_ecs_bitset_true_(&exclude_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
    }
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    bxr_ecs_system_data_t_ *system_data = &ecs->systems[ecs->active_system];

    bxr_ecs_bitset_t_ exclude_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->exclude_bits);

    bxr_ecs_bitset_t_ require_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->require_bits);

    bxr_sparse_set_t *set = system_data->entity_ids;

    // If not excluded and required then the entity should be added to the
    // system
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_equal_(&require_overlap,
                                 &system_data->require_bits)) {
      size_t count    = bxr_sparse_set_count(set);
      size_t capacity = bxr_sparse_set_capacity(set);

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

          bxr_ecs_push_add_entity_(ecs, entity);
        }
      }
    }
    // If the added component causes the entity to be excluded, check if it was
    // in the system and remove it
    else if (bxr_ecs_bitset_true_(&exclude_overlap)) {
      if (bxr_sparse_set_contains(set, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        bxr_ecs_push_remove_entity_(ecs, entity);
      }
    }
  }

  return component_ptr;
}

void *
bxr_ecs_entity_get_component(bxr_ecs_t *ecs,
                             bxr_ecs_entity_t entity,
                             bxr_ecs_component_t component)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);
  BXR_ASSERT(ecs->entities[entity].ready);

  bxr_ecs_component_array_t_ *component_array
      = &ecs->component_arrays[component];

  return (Uint8 *)component_array->data + (component_array->size * entity);
}

void
bxr_ecs_entity_remove_component(bxr_ecs_t *ecs,
                                bxr_ecs_entity_t entity,
                                bxr_ecs_component_t component)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);
  BXR_ASSERT(ecs->entities[entity].ready);

  bxr_ecs_entity_data_t_ *entity_data = &ecs->entities[entity];

  bxr_ecs_bitset_t_ component_bits;
  BXR_MEMSET(&component_bits, 0, sizeof(bxr_ecs_bitset_t_));
  bxr_ecs_bitset_set_(&component_bits, component);

  bxr_ecs_bitset_t_ next_component_bits
      = bxr_ecs_bitset_copy_(&entity_data->component_bits);
  bxr_ecs_bitset_unset_(&next_component_bits, component);

  for (bxr_ecs_id_t_ system_id = 0; system_id < ecs->system_count;
       system_id++) {

    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    bxr_ecs_system_data_t_ *system_data = &ecs->systems[system_id];

    bxr_ecs_bitset_t_ exclude_overlap
        = bxr_ecs_bitset_and_(&component_bits, &system_data->exclude_bits);
    bxr_ecs_bitset_t_ require_overlap
        = bxr_ecs_bitset_and_(&component_bits, &system_data->require_bits);

    // If not excluded and required then the entity is in the system and should
    // be removed.
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_true_(&require_overlap)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
    }
    // If the entity was excluded because of it's removed component, check if it
    // now eligible to be added to the system and add it
    else if (bxr_ecs_bitset_true_(&exclude_overlap)
             && bxr_ecs_bitset_equal_(&next_component_bits,
                                      &system_data->require_bits)) {
      if (bxr_sparse_set_insert(system_data->entity_ids, entity)) {
        if (system_data->add_cb)
          system_data->add_cb(ecs, entity, system_data->udata);
      }
    }
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    bxr_ecs_system_data_t_ *system_data = &ecs->systems[ecs->active_system];

    bxr_ecs_bitset_t_ exclude_overlap
        = bxr_ecs_bitset_and_(&component_bits, &system_data->exclude_bits);
    bxr_ecs_bitset_t_ require_overlap
        = bxr_ecs_bitset_and_(&component_bits, &system_data->require_bits);

    // If not excluded and required then the entity is in the system and should
    // be removed.
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_true_(&require_overlap)) {
      if (bxr_sparse_set_contains(system_data->entity_ids, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        bxr_ecs_push_remove_entity_(ecs, entity);
      }
    }
    // If the entity was excluded because of it's removed component, check if it
    // now eligible to be added to the system and add it
    else if (bxr_ecs_bitset_true_(&exclude_overlap)
             && bxr_ecs_bitset_equal_(&next_component_bits,
                                      &system_data->require_bits)) {
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

          bxr_ecs_push_add_entity_(ecs, entity);
        }
      }
    }
  }

  bxr_ecs_component_data_t_ *component_data = &ecs->components[component];

  if (component_data->destroy_cb) {
    void *component_ptr = bxr_ecs_entity_get_component(ecs, entity, component);
    component_data->destroy_cb(ecs, entity, component_ptr);
  }

  // Apply the component bit unset to the entity
  bxr_ecs_bitset_unset_(&entity_data->component_bits, component);
}

void
bxr_ecs_entity_destroy(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(ecs->entities[entity].active);

  bxr_ecs_entity_data_t_ *entity_data = &ecs->entities[entity];

  // Remove entity from systems
  for (bxr_ecs_id_t_ system_id = 0; system_id < ecs->system_count;
       system_id++) {

    // Running system will be handled below
    if (ecs->active_system == (int)system_id)
      continue;

    bxr_ecs_system_data_t_ *system_data = &ecs->systems[system_id];

    bxr_ecs_bitset_t_ exclude_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->exclude_bits);
    bxr_ecs_bitset_t_ require_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->require_bits);

    // If not excluded and required then the entity should be removed from the
    // system
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_equal_(&require_overlap,
                                 &system_data->require_bits)) {
      if (bxr_sparse_set_remove(system_data->entity_ids, entity)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);
      }
    }
  }

  // If a system is running, manage entity's stacks accordingly
  if (ecs->active_system >= 0) {
    bxr_ecs_system_data_t_ *system_data = &ecs->systems[ecs->active_system];

    bxr_ecs_bitset_t_ exclude_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->exclude_bits);
    bxr_ecs_bitset_t_ require_overlap = bxr_ecs_bitset_and_(
        &entity_data->component_bits, &system_data->require_bits);

    // If not excluded and required then the entity should be removed from the
    // system
    if (!bxr_ecs_bitset_true_(&exclude_overlap)
        && bxr_ecs_bitset_equal_(&require_overlap,
                                 &system_data->require_bits)) {

      if (bxr_sparse_set_contains(system_data->entity_ids, entity, NULL)) {
        if (system_data->remove_cb)
          system_data->remove_cb(ecs, entity, system_data->udata);

        bxr_ecs_push_destroy_entity_(ecs, entity);

        entity_data->ready  = false;
        entity_data->active = true;
      }
    }
  }

  bxr_ecs_destruct_(ecs, entity);

  // If no system is running, clean up the entity immediately and push it to the
  // free stack for re-use
  if (ecs->active_system < 0) {
    bxr_ecs_push_free_entity_(ecs, entity);
    BXR_MEMSET(entity_data, 0, sizeof(bxr_ecs_entity_data_t_));
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
  BXR_ASSERT(ecs);
  BXR_ASSERT(ecs->system_count < BXR_ECS_MAX_SYSTEM);

  bxr_ecs_system_t id = ecs->system_count;

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[id];

  system_data->active     = true;
  system_data->mask       = mask;
  system_data->entity_ids = bxr_sparse_set_create(
      ecs->entity_count); // Start with capacity equal to max entities
  system_data->logic_cb     = logic_cb;
  system_data->add_cb       = add_cb;
  system_data->remove_cb    = remove_cb;
  system_data->require_bits = (bxr_ecs_bitset_t_){ 0 };
  system_data->exclude_bits = (bxr_ecs_bitset_t_){ 0 };
  system_data->udata        = udata;

  ecs->system_count++;

  return id;
}

void
bxr_ecs_system_require_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t system,
                                 bxr_ecs_component_t component)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];

  bxr_ecs_bitset_set_(&system_data->require_bits, component);
}

void
bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t system,
                                 bxr_ecs_component_t component)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);
  BXR_ASSERT(component < BXR_ECS_MAX_COMPONENT);
  BXR_ASSERT(component < ecs->component_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];

  bxr_ecs_bitset_set_(&system_data->exclude_bits, component);
}

void
bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];
  system_data->active                 = true;
}

void
bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];
  system_data->active                 = false;
}

void
bxr_ecs_system_set_callback(bxr_ecs_t *ecs,
                            bxr_ecs_system_t system,
                            bxr_ecs_system_logic_cb logic_cb,
                            bxr_ecs_system_add_cb add_cb,
                            bxr_ecs_system_remove_cb remove_cb)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];

  system_data->logic_cb  = logic_cb;
  system_data->add_cb    = add_cb;
  system_data->remove_cb = remove_cb;
}

void
bxr_ecs_system_set_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system, void *udata)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  ecs->systems[system].udata = udata;
}

void *
bxr_ecs_system_get_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  return ecs->systems[system].udata;
}

void
bxr_ecs_system_set_mask(bxr_ecs_t *ecs,
                        bxr_ecs_system_t system,
                        bxr_ecs_mask_t mask)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  ecs->systems[system].mask = mask;
}

bxr_ecs_mask_t
bxr_ecs_system_get_mask(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  return ecs->systems[system].mask;
}

size_t
bxr_ecs_system_get_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t system)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  return bxr_sparse_set_count(ecs->systems[system].entity_ids);
}

int
bxr_ecs_system_run(bxr_ecs_t *ecs, bxr_ecs_system_t system, bxr_ecs_mask_t mask)
{
  BXR_ASSERT(ecs);
  BXR_ASSERT(system < BXR_ECS_MAX_SYSTEM);
  BXR_ASSERT(system < ecs->system_count);

  bxr_ecs_system_data_t_ *system_data = &ecs->systems[system];
  if (!system_data->active)
    return 0;

  if (system_data->mask != 0 && !(system_data->mask & mask))
    return 0;

  ecs->active_system = system;

  size_t *entities      = bxr_sparse_set_get_dense(system_data->entity_ids);
  size_t entities_count = bxr_sparse_set_count(system_data->entity_ids);

  int retval = system_data->logic_cb(
      ecs, entities, entities_count, system_data->udata);

  bxr_ecs_flush_add_entity_(ecs, system);
  bxr_ecs_flush_remove_entity_(ecs, system);
  bxr_ecs_flush_destroy_entity_(ecs, system);

  ecs->active_system = -1;

  return retval;
}

int
bxr_ecs_systems_run(bxr_ecs_t *ecs, bxr_ecs_mask_t mask)
{
  BXR_ASSERT(ecs);

  for (bxr_ecs_id_t_ system_id = 0; system_id < ecs->system_count;
       system_id++) {

    int retval = bxr_ecs_system_run(ecs, system_id, mask);

    if (retval != 0) {
      return retval;
    }
  }

  return 0;
}
