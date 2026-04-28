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
  bxr_ecs_component_make_cb make_cb;
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
  bxr_ecs_logic_cb logic_cb;
  bxr_ecs_system_added_cb added_cb;
  bxr_ecs_system_removed_cb removed_cb;
  bxr_bitset_t *require_bits;
  bxr_bitset_t *exclude_bits;
  void *udata;
} _bxr_ecs_system_data_t;

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
                         bxr_ecs_component_make_cb make_cb,
                         bxr_ecs_component_destroy_cb destroy_cb)
{
  SDL_assert(ecs);
  SDL_assert(ecs->component_count < ecs->max_components);

  bxr_ecs_component_t id = ecs->component_count;

  _bxr_ecs_component_data_t *comp_data = &ecs->component_data[id];

  comp_data->make_cb    = make_cb;
  comp_data->destroy_cb = destroy_cb;

  ecs->component_count++;

  return id;
}

bxr_ecs_system_t
bxr_ecs_define_system(bxr_ecs_t *ecs,
                      bxr_ecs_mask_t mask,
                      bxr_ecs_logic_cb logic_cb,
                      bxr_ecs_system_added_cb added_cb,
                      bxr_ecs_system_removed_cb removed_cb,
                      void *udata)
{
  SDL_assert(ecs);
  SDL_assert(ecs->system_count < ecs->max_systems);

  bxr_ecs_system_t id = ecs->system_count;

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[id];

  sys_data->active       = true;
  sys_data->mask         = mask;
  sys_data->entity_ids   = bxr_sparse_set_make(ecs->max_entities);
  sys_data->logic_cb     = logic_cb;
  sys_data->added_cb     = added_cb;
  sys_data->removed_cb   = removed_cb;
  sys_data->require_bits = bxr_bitset_make(ecs->max_components);
  sys_data->exclude_bits = bxr_bitset_make(ecs->max_components);
  sys_data->udata        = udata;

  ecs->system_count++;

  return id;
}

void
bxr_ecs_system_require_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t sys,
                                 bxr_ecs_component_t comp)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);
  SDL_assert(comp < ecs->max_components);
  SDL_assert(comp < ecs->component_count);

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[sys];

  bxr_bitset_set(sys_data->require_bits, comp);
}

void
bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t sys,
                                 bxr_ecs_component_t comp)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);
  SDL_assert(comp < ecs->max_components);
  SDL_assert(comp < ecs->component_count);

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[sys];

  bxr_bitset_set(sys_data->exclude_bits, comp);
}

void
bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t sys)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[sys];
  sys_data->active                 = true;
}

void
bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t sys)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[sys];
  sys_data->active                 = false;
}

void
bxr_ecs_system_set_callback(bxr_ecs_t *ecs,
                            bxr_ecs_system_t sys,
                            bxr_ecs_logic_cb logic_cb,
                            bxr_ecs_system_added_cb added_cb,
                            bxr_ecs_system_removed_cb removed_cb)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);

  _bxr_ecs_system_data_t *sys_data = &ecs->system_data[sys];

  sys_data->logic_cb   = logic_cb;
  sys_data->added_cb   = added_cb;
  sys_data->removed_cb = removed_cb;
}

void
bxr_ecs_set_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t sys, void *udata)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);

  ecs->system_data[sys].udata = udata;
}

void *
bxr_ecs_get_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t sys)
{
  SDL_assert(ecs);
  SDL_assert(sys < ecs->max_systems);
  SDL_assert(sys < ecs->system_count);

  return ecs->system_data[sys].udata;
}
