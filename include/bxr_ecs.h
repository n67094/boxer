/**
 * File `bxr_ecs.h`.
 *
 * # Entity Component System.
 *
 * Overview:
 *
 * - Entities are IDs.
 * - Components are plain data blocks attached to entities.
 * - Systems run logic over every entity that matches their required and
 * excluded components.
 *
 * Typical usage: `bxr_ecs_create()`, define components with
 * `bxr_ecs_component_define()`, define systems with `bxr_ecs_system_define()`
 * and set their requirements with `bxr_ecs_system_require_component()` /
 * `bxr_ecs_system_exclude_component()`, then create entities and add
 * components. Each frame call `bxr_ecs_systems_run()`. Free everything with
 * `bxr_ecs_destroy()`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * This is an altered version of pico_ecs by James McLean
 * (https://github.com/empyreanx/pico_headers).
 * Copyright (c) 2025 James McLean.
 */

#ifndef BXR_ECS_H_
#define BXR_ECS_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

typedef size_t bxr_ecs_entity_t;
typedef size_t bxr_ecs_component_t;
typedef size_t bxr_ecs_system_t;

typedef size_t bxr_ecs_mask_t;

// TODO add assert for those variable
#define BXR_ECS_INVALID_ENTITY ((bxr_ecs_entity_t)(-1))
#define BXR_ECS_INVALID_COMPONENT ((bxr_ecs_component_t)(-1))
#define BXR_ECS_INVALID_SYSTEM ((bxr_ecs_component_t)(-1))

/**
 * ECS API
 */

typedef struct bxr_ecs_s bxr_ecs_t;

/**
 * Create a new ECS instance.
 *
 * `entity_count` initial entity capacity; 0 uses `BXR_ECS_DEFAULT_ENTITY`. The
 * capacity doubles when more entities are created and never shrinks.
 *
 * `return` the ECS instance, or NULL on allocation failure.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller must free the result with `bxr_ecs_destroy()`.
 */
BXR_API bxr_ecs_t *bxr_ecs_create(size_t entity_count);

/**
 * Destroy an ECS instance and free all its entities, components and systems.
 *
 * `ecs` the ECS instance to destroy
 */
BXR_API void bxr_ecs_destroy(bxr_ecs_t *ecs);

/**
 * Remove all entities, keeping component and system definitions.
 *
 * `ecs` the ECS instance to reset; must not be NULL.
 */
BXR_API void bxr_ecs_reset(bxr_ecs_t *ecs);

/**
 * ECS component API
 */

/**
 * Callback called when a component is added to an entity.
 *
 * `ecs` the ECS instance the component belongs to.
 * `entity` the entity the component is added to.
 * `component_ptr` the zeroed component data, to initialize.
 * `args` the `args` given to `bxr_ecs_entity_add_component()`; may be NULL.
 */
typedef void (*bxr_ecs_component_add_cb)(bxr_ecs_t *ecs,
                                         bxr_ecs_entity_t entity,
                                         void *component_ptr,
                                         void *args);

/**
 * Callback called when a component is removed from an entity.
 *
 * `ecs` the ECS instance the component belongs to.
 * `entity` the entity the component is removed from.
 * `component_ptr` the component data, still valid during the call.
 */
typedef void (*bxr_ecs_component_remove_cb)(bxr_ecs_t *ecs,
                                            bxr_ecs_entity_t entity,
                                            void *component_ptr);

/**
 * Define a new component type.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `component_size` size of the component data in bytes.
 * `add_cb` called when the component is added to an entity; may be NULL.
 * `remove_cb` called when the component is removed from an entity; may be
 * NULL.
 *
 * `return` the component ID or `BXR_ECS_INVALID_COMPONENT` on failure.
 * Call `bxr_get_error()` for more information.
 *
 * **NOTE:** at most `BXR_ECS_MAX_COMPONENT` components can be
 * defined.
 */
BXR_API bxr_ecs_component_t
bxr_ecs_component_define(bxr_ecs_t *ecs,
                         size_t component_size,
                         bxr_ecs_component_add_cb add_cb,
                         bxr_ecs_component_remove_cb remove_cb);

/**
 * ECS Entity API
 */

/**
 * Create a new entity with no component.
 *
 * `ecs` the ECS instance; must not be NULL.
 *
 * `return` the entity ID, or `BXR_ECS_INVALID_ENTITY` on allocation failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bxr_ecs_entity_t bxr_ecs_entity_create(bxr_ecs_t *ecs);

/**
 * Check if an entity is alive and not queued for destruction.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to check.
 *
 * `return` true if the entity is ready, false otherwise.
 */
BXR_API bool bxr_ecs_entity_is_ready(const bxr_ecs_t *ecs,
                                     bxr_ecs_entity_t entity);

/**
 * Check if an entity has a component.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to check; must be ready.
 * `component` the component to look for; must be defined.
 *
 * `return` true if the entity has the component, false otherwise.
 */
BXR_API bool bxr_ecs_entity_has_component(const bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          bxr_ecs_component_t component);

/**
 * Add a component to an entity.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to modify; must be ready.
 * `component` the component to add; must be defined.
 * `args` passed as is to the component's add callback; may be NULL.
 *
 * `return` the component data, or NULL on allocation failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API void *bxr_ecs_entity_add_component(bxr_ecs_t *ecs,
                                           bxr_ecs_entity_t entity,
                                           bxr_ecs_component_t component,
                                           void *args);

/**
 * Get the data of an entity's component.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to query; must be ready.
 * `component` the component to get; must be defined.
 *
 * `return`
 */
BXR_API void *bxr_ecs_entity_get_component(bxr_ecs_t *ecs,
                                           bxr_ecs_entity_t entity,
                                           bxr_ecs_component_t component);

/**
 * Remove a component from an entity.
 *
 * The entity is:
 * - removed from every system it no longer matches
 * - added to every system is no longer excluded from
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to modify; must be ready.
 * `component` the component to remove; must be defined.
 */
BXR_API void bxr_ecs_entity_remove_component(bxr_ecs_t *ecs,
                                             bxr_ecs_entity_t entity,
                                             bxr_ecs_component_t component);

/**
 * Destroy an entity.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `entity` the entity to destroy; must be alive.
 */
BXR_API void bxr_ecs_entity_destroy(bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

/**
 * ECS System API
 */

/**
 * Callback called when a system is run.
 *
 * `ecs` the ECS instance the system belongs to.
 * `entities` the entities matching the system;
 * `entity_count` the number of entities in `entities`.
 * `udata` the user data of the system.
 *
 * `return` 0 on success, non-zero on failure.
 */
typedef int (*bxr_ecs_system_logic_cb)(bxr_ecs_t *ecs,
                                       const bxr_ecs_entity_t *entities,
                                       size_t entity_count,
                                       void *udata);

/**
 * Callback called when an entity is added to a system.
 *
 * `ecs` the ECS instance the system belongs to.
 * `entity` the entity added to the system.
 * `udata` the user data of the system.
 */
typedef void (*bxr_ecs_system_add_cb)(bxr_ecs_t *ecs,
                                      bxr_ecs_entity_t entity,
                                      void *udata);

/**
 * Callback called when an entity is removed from a system.
 *
 * `ecs` the ECS instance the system belongs to.
 * `entity` the entity removed from the system.
 * `udata` the user data of the system.
 */
typedef void (*bxr_ecs_system_remove_cb)(bxr_ecs_t *ecs,
                                         bxr_ecs_entity_t entity,
                                         void *udata);

/**
 * Define a new system, enabled by default.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `mask` group bits (a.k.a. tags) used to filter runs; 0 means the system runs
 * for any mask.
 * `logic_cb` called when the system is run; must not be NULL.
 * `add_cb` called when an entity is added to the system; may be NULL.
 * `remove_cb` called when an entity is removed from the system; may be NULL.
 * `udata` user data passed to the callbacks; may be NULL. Not owned.
 *
 * `return` the system ID or `BXR_ECS_INVALID_SYSTEM` on allocation failure.
 * Call `bxr_get_error()` for more information.
 *
 * **NOTE:** At most `BXR_ECS_MAX_SYSTEM` systems can be defined.
 */
BXR_API bxr_ecs_system_t
bxr_ecs_system_define(bxr_ecs_t *ecs,
                      bxr_ecs_mask_t mask,
                      bxr_ecs_system_logic_cb logic_cb,
                      bxr_ecs_system_add_cb add_cb,
                      bxr_ecs_system_remove_cb remove_cb,
                      void *udata);

/**
 * Require a component for an entity to belong to a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to modify; must be defined .
 * `component` the required component; must be defined.
 */
BXR_API void bxr_ecs_system_require_component(bxr_ecs_t *ecs,
                                              bxr_ecs_system_t system,
                                              bxr_ecs_component_t component);

/**
 * Exclude entities holding a component from a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to modify; must be defined .
 * `component` the excluded component; must be defined.
 */
BXR_API void bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
                                              bxr_ecs_system_t system,
                                              bxr_ecs_component_t component);

/**
 * Enable a system so `bxr_ecs_system_run()` and `bxr_ecs_systems_run()` run it.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to enable; must be defined.
 */
BXR_API void bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t system);

/**
 * Disable a system so `bxr_ecs_system_run()` and `bxr_ecs_systems_run()` skip
 * it.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to disable; must be defined.
 */
BXR_API void bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t system);

/**
 * Replace the callbacks of a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to modify; must be defined.
 * `logic_cb` called when the system is run; must not be NULL.
 * `add_cb` called when an entity is added to the system; may be NULL.
 * `remove_cb` called when an entity is removed from the system; may be NULL.
 */
BXR_API void bxr_ecs_system_set_callbacks(bxr_ecs_t *ecs,
                                          bxr_ecs_system_t system,
                                          bxr_ecs_system_logic_cb logic_cb,
                                          bxr_ecs_system_add_cb add_cb,
                                          bxr_ecs_system_remove_cb remove_cb);

/**
 * Set the user data passed to a system's callbacks.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to modify; must be defined.
 * `udata` the user data; may be NULL. Not owned.
 */
BXR_API void
bxr_ecs_system_set_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system, void *udata);

/**
 * Get the user data of a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to query; must be defined.
 *
 * `return` the user data, or NULL if none was set.
 */
BXR_API void *bxr_ecs_system_get_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system);

/**
 * Set the group mask (a.k.a. tags) of a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to modify; must be defined.
 * `mask` the group bits; 0 means the system runs for any mask.
 */
BXR_API void bxr_ecs_system_set_mask(bxr_ecs_t *ecs,
                                     bxr_ecs_system_t system,
                                     bxr_ecs_mask_t mask);

/**
 * Get the group mask (a.k.a. tags) of a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to query; must be defined.
 *
 * `return` the group bits of the system.
 */
BXR_API bxr_ecs_mask_t bxr_ecs_system_get_mask(bxr_ecs_t *ecs,
                                               bxr_ecs_system_t system);

/**
 * Get the number of entities belonging to a system.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to query; must be defined.
 *
 * `return` the entity count.
 */
BXR_API size_t bxr_ecs_system_get_entity_count(bxr_ecs_t *ecs,
                                               bxr_ecs_system_t system);

/**
 * Run a system's logic callback over its entities.
 *
 * The system runs only if it is enabled and its mask is 0 or shares a bit with
 * `mask`.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `system` the system to run; must be defined.
 * `mask` the groups to run.
 *
 * `return` the logic callback's result, or 0 if the system was skipped.
 *
 * **Note**: must not be called from a system callback.
 */
BXR_API int bxr_ecs_system_run(bxr_ecs_t *ecs,
                               bxr_ecs_system_t system,
                               bxr_ecs_mask_t mask);

/**
 * Run every system with `bxr_ecs_system_run()`, in definition order.
 *
 * `ecs` the ECS instance; must not be NULL.
 * `mask` the groups to run.
 *
 * `return` 0 if all systems ran successfully, or a non-zero if a system's logic
 * callback returned non-zero.
 *
 * **Note**: must not be called from a system callback.
 */
BXR_API int bxr_ecs_systems_run(bxr_ecs_t *ecs, bxr_ecs_mask_t mask);

#endif // BXR_ECS_H_
