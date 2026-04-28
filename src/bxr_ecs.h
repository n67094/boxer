/**
 * File `bxr_ecs.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ECS_H_
#define BXR_ECS_H_

#include <SDL3/SDL.h>

typedef Uint32 bxr_ecs_entity_t;
typedef Uint32 bxr_ecs_component_t;
typedef Uint32 bxr_ecs_system_t;

typedef Uint64 bxr_ecs_mask_t;

/**
 * Description for creating an ECS instance.
 *
 * `max_entities` is the maximum number of entities that can be created in the
 * ECS instance. (default: BXR_ECS_DEFAULT_MAX_ENTITIES)
 */
typedef struct bxr_ecs_desc_s
{
  Uint32 max_entities;
  Uint32 max_components;
  Uint32 max_systems;
} bxr_ecs_desc_t;

typedef struct bxr_ecs_s bxr_ecs_t;

/**
 * Create a new ECS instance with the given description.
 *
 * `desc` is the description of the ECS instance to create.
 *
 * `return` a pointer to the created ECS instance, or NULL if the ECS instance
 * could not be created.
 *
 * The caller is responsible for destroying the returned ECS instance using
 * `bxr_ecs_destroy` when it is no longer needed.
 */
bxr_ecs_t *bxr_ecs_make(const bxr_ecs_desc_t *desc);

/**
 * Destroy the given ECS instance and free its memory.
 *
 * `ecs` is the ECS instance to destroy.
 */
void bxr_ecs_destroy(bxr_ecs_t *ecs);

/**
 * Reset the ECS instance, removing all entities, preserving systems and
 * components.
 *
 * `ecs` is the ECS instance to reset.
 */
void bxr_ecs_reset(bxr_ecs_t *ecs);

/**
 * Callback called when a component is created.
 *
 * `ecs` is the ECS instance the component belongs to.
 *
 * `entity` is the entity the component belongs to.
 *
 * `component_ptr` is a pointer to the component data.
 *
 * `args` is the arguments passed to `bxr_ecs_add_component_to_entity` when
 * adding the component to an entity.
 */
typedef void (*bxr_ecs_component_make_cb)(bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          void *component_ptr,
                                          void *args);

/**
 * Callback called when a component is destroyed.
 *
 * `ecs` is the ECS instance the component belongs to.
 *
 * `entity` is the entity the component belongs to.
 *
 * `component_ptr` is a pointer to the component data.
 */
typedef void (*bxr_ecs_component_destroy_cb)(bxr_ecs_t *ecs,
                                             bxr_ecs_entity_t entity,
                                             void *component_ptr);

/**
 * Define a new component type in the ECS instance.
 *
 * `ecs` is the ECS instance to define the component in.
 *
 * `make_cb` called when a component of this type is created. (NULL if no
 * callback is needed).
 *
 * `destroy_cb` called when a component of this type is
 * destroyed. (NULL if no callback is needed).
 *
 * `return` the ID of the defined component, or an invalid ID if the component
 * could not be defined.
 */
bxr_ecs_component_t
bxr_ecs_define_component(bxr_ecs_t *ecs,
                         bxr_ecs_component_make_cb make_cb,
                         bxr_ecs_component_destroy_cb destroy_cb);

/**
 * Callback called when a system is run.
 *
 * `ecs` is the ECS instance the system belongs to.
 *
 * `entities` is an array of entities that match the system's component mask.
 *
 * `entity_count` is the number of entities in the `entities` array.
 *
 * `udata` is the user data associated with the system.
 *
 * `return` 0 on success, or a non-zero error code on failure.
 */
typedef int (*bxr_ecs_logic_cb)(bxr_ecs_t *ecs,
                                bxr_ecs_entity_t *entities,
                                size_t entity_count,
                                void *udata);

/**
 * Callback called when an entity is added to a system.
 *
 * `ecs` is the ECS instance the system belongs to.
 *
 * `entity` is the entity that was added to the system.
 *
 * `udata` is the user data associated with the system.
 */
typedef void (*bxr_ecs_system_added_cb)(bxr_ecs_t *ecs,
                                        bxr_ecs_entity_t entity,
                                        void *udata);

/**
 * Callback called when an entity is removed from a system.
 *
 * `ecs` is the ECS instance the system belongs to.
 *
 * `entity` is the entity that was removed from the system.
 *
 * `udata` is the user data associated with the system.
 */
typedef void (*bxr_ecs_system_removed_cb)(bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          void *udata);

/**
 * Define a new system in the ECS instance.
 *
 * `ecs` is the ECS instance to define the system in.
 *
 * `mask` is the system mask (a.k.a tags) to associate with the system.
 *
 * `logic_cb` is the callback to call when the system is run.
 *
 * `added_cb` is the callback to call when an entity is added to the system.
 *
 * `removed_cb` is the callback to call when an entity is removed from the
 * system.
 *
 * `udata` is the user data to associate with the system (can be NULL).
 *
 * `return` the ID of the defined system, or an invalid ID if the system could
 * not be defined.
 */
bxr_ecs_system_t bxr_ecs_define_system(bxr_ecs_t *ecs,
                                       bxr_ecs_mask_t mask,
                                       bxr_ecs_logic_cb logic_cb,
                                       bxr_ecs_system_added_cb added_cb,
                                       bxr_ecs_system_removed_cb removed_cb,
                                       void *udata);

/**
 * Require a component for a system.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to require the component for.
 *
 * `comp` is the component to require for the system.
 */
void bxr_ecs_system_require_component(bxr_ecs_t *ecs,
                                      bxr_ecs_system_t sys,
                                      bxr_ecs_component_t comp);

/**
 * Exclude a component for a system.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to exclude the component for.
 *
 * `comp` is the component to exclude for the system.
 */
void bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
                                      bxr_ecs_system_t sys,
                                      bxr_ecs_component_t comp);

/**
 * Enable a system, allowing it to be run when `bxr_ecs_run_system` or
 * `ecs_run_systems` is called.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to enable.
 */
void bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

/**
 * Disable a system, preventing it from being run when `bxr_ecs_run_system` or
 * `ecs_run_systems` is called.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to disable.
 */
void bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

/**
 * Set the callbacks for a system.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to set the callbacks for.
 *
 * `logic_cb` is the callback to call when the system is run.
 *
 * `added_cb` is the callback to call when an entity is added to the system.
 *
 * `removed_cb` is the callback to call when an entity is removed from the
 * system.
 */
void bxr_ecs_system_set_callback(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t sys,
                                 bxr_ecs_logic_cb logic_cb,
                                 bxr_ecs_system_added_cb added_cb,
                                 bxr_ecs_system_removed_cb removed_cb);

/**j
 * Set the user data for a system.
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to set the user data for.
 *
 * `udata` is the user data to associate with the system (can be NULL).
 */
void
bxr_ecs_set_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t sys, void *udata);

/**
 * Get the user data for a system.
 *
 * `ecs` is the ECS instance to query.
 *
 * `sys` is the system to get the user data for.
 *
 * `return` the user data associated with the system, or NULL if no user data
 * is associated with the system.
 */
void *bxr_ecs_get_system_udata(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

/**
 * Set the system mask (a.k.a tags).
 *
 * `ecs` is the ECS instance to modify.
 *
 * `sys` is the system to set the mask for.
 *
 * `mask` is the mask to set for the system.
 */
void bxr_ecs_set_system_mask(bxr_ecs_t *ecs,
                             bxr_ecs_system_t sys,
                             bxr_ecs_mask_t mask);

bxr_ecs_mask_t bxr_ecs_get_system_mask(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

size_t bxr_ecs_get_system_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

bxr_ecs_entity_t *bxr_ecs_make_entity(bxr_ecs_t *ecs);

bool bxr_ecs_is_entity_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

bool bxr_ecs_has_entity(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

void bxr_ecs_destroy_entity(bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

void *bxr_ecs_entity_add_component(bxr_ecs_t *ecs,
                                   bxr_ecs_entity_t entity,
                                   bxr_ecs_component_t component,
                                   void *args);

void *bxr_ecs_entity_get_component(const bxr_ecs_t *ecs,
                                   bxr_ecs_entity_t entity,
                                   bxr_ecs_component_t component);

void bxr_ecs_entity_remove_component(bxr_ecs_t *ecs,
                                     bxr_ecs_entity_t entity,
                                     bxr_ecs_component_t component);

int
bxr_ecs_run_system(bxr_ecs_t *ecs, bxr_ecs_system_t sys, bxr_ecs_mask_t mask);

int ecs_run_systems(bxr_ecs_t *ecs, bxr_ecs_mask_t mask);

#endif // BXR_ECS_H_
