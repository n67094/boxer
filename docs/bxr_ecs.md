<a name="line-1"></a>
File `bxr_ecs.h`.

This ECS is based on "pico_ecs.h" Copyright (c) 2025 James Mclean.

Copyright (c) 2026 nsix. All rights reserved.

<a name="line-22"></a>
ECS API

<a name="line-28"></a>

```cpp
bxr_ecs_t *bxr_ecs_create(size_t entity_count)
```

Create a new ECS instance with the given description.

`entity_count` is the starting number of entities the ECS instance can hold.
The ECS instance will automatically grow if more entities are created, but
will never shrink.

`return` a pointer to the created ECS instance, or NULL if an error occurred.
Use `bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned ECS instance using
`bxr_ecs_destroy` when it is no longer needed.
---

<a name="line-43"></a>

```cpp
void bxr_ecs_destroy(bxr_ecs_t *ecs)
```

Destroy the given ECS instance and free its memory.

`ecs` is the ECS instance to destroy.
---

<a name="line-50"></a>

```cpp
void bxr_ecs_reset(bxr_ecs_t *ecs)
```

Reset the ECS instance, removing all entities, preserving systems and
components.

`ecs` is the ECS instance to reset.
---

<a name="line-58"></a>
ECS component API

<a name="line-62"></a>

```cpp
typedef void (*bxr_ecs_component_create_cb)(bxr_ecs_t *ecs,
```

Callback called when a component is created.

`ecs` is the ECS instance the component belongs to.

`entity` is the entity the component belongs to.

`component_ptr` is a pointer to the component data.

`args` is the arguments passed to `bxr_ecs_entity_add_component` when adding
the component (can be NULL).
---

<a name="line-79"></a>

```cpp
typedef void (*bxr_ecs_component_destroy_cb)(bxr_ecs_t *ecs,
```

Callback called when a component is destroyed.

`ecs` is the ECS instance the component belongs to.

`entity` is the entity the component belongs to.

`component_ptr` is a pointer to the component data.
---

<a name="line-92"></a>

```cpp
bxr_ecs_component_t
```

Define a new component type in the ECS instance.

`ecs` is the ECS instance to define the component in.

`component_size` is the size of the component data in bytes.

`create_cb` called when a component of this type is created. (NULL if no
callback is needed).

`destroy_cb` called when a component of this type is
destroyed. (NULL if no callback is needed).

`return` the ID of the defined component, or an invalid ID if the component
could not be defined.
---

<a name="line-114"></a>
ECS Entity API

<a name="line-118"></a>

```cpp
bxr_ecs_entity_t bxr_ecs_entity_create(bxr_ecs_t *ecs)
```

Create a new entity in the ECS instance.

`ecs` is the ECS instance to create the entity in.

`return` the ID of the created entity, or BXR_ECS_INVALID_ENTITY if the
entity could not be created. Use `bxr_error_get` to get more information
about the error.
---

<a name="line-129"></a>

```cpp
bool bxr_ecs_entity_is_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
```

Return `true` if the entity is active and has not been queued for
destruction, `false` otherwise.

`ecs` is the ECS instance to query.

`entity` is the entity to check.
---

<a name="line-139"></a>

```cpp
bool bxr_ecs_entity_has_component(const bxr_ecs_t *ecs,
```

Test if an entity has a component.

`ecs` is the ECS instance to query.

`entity` is the entity to check.

`component` is the component to check for.

`return` `true` if the entity has the component, `false` otherwise.
---

<a name="line-154"></a>

```cpp
void *bxr_ecs_entity_add_component(bxr_ecs_t *ecs,
```

Add a component to an entity.

`ecs` is the ECS instance to modify.

`entity` is the entity to add the component to.

`component` is the component to add to the entity.

`args` is the arguments to pass to the component's create callback when
creating the component data for the entity (can be NULL).

`return` a pointer to the created component data, or NULL if an error
occurred. Use `bxr_error_get` to get more information about the error.
---

<a name="line-174"></a>

```cpp
void *bxr_ecs_entity_get_component(bxr_ecs_t *ecs,
```

Get a pointer to a component of an entity.

`ecs` is the ECS instance to query.

`entity` is the entity to get the component from.

`component` is the component to get from the entity.

`return` a pointer to the component data, or NULL if the entity does not have
the component.
---

<a name="line-190"></a>

```cpp
void bxr_ecs_entity_remove_component(bxr_ecs_t *ecs,
```

Remove a component from an entity.

`ecs` is the ECS instance to modify.

`entity` is the entity to remove the component from.

`component` is the component to remove from the entity.
---

<a name="line-203"></a>

```cpp
void bxr_ecs_entity_destroy(bxr_ecs_t *ecs, bxr_ecs_entity_t entity)
```

Destroy an entity, free it's resources and push it's id in the entity
stack for re-use.

`ecs` is the ECS instance to query.

`entity` is the entity to destroy
---

<a name="line-213"></a>
ECS System API

<a name="line-217"></a>

```cpp
typedef int (*bxr_ecs_system_logic_cb)(bxr_ecs_t *ecs,
```

Callback called when a system is run.

`ecs` is the ECS instance the system belongs to.

`entities` is an array of entities that match the system's component mask.

`entity_count` is the number of entities in the `entities` array.

`udata` is the user data associated with the system.

`return` 0 on success, or a non-zero error code on failure.
---

<a name="line-235"></a>

```cpp
typedef void (*bxr_ecs_system_add_cb)(bxr_ecs_t *ecs,
```

Callback called when an entity is added to a system.

`ecs` is the ECS instance the system belongs to.

`entity` is the entity that was added to the system.

`udata` is the user data associated with the system.
---

<a name="line-248"></a>

```cpp
typedef void (*bxr_ecs_system_remove_cb)(bxr_ecs_t *ecs,
```

Callback called when an entity is removed from a system.

`ecs` is the ECS instance the system belongs to.

`entity` is the entity that was removed from the system.

`udata` is the user data associated with the system.
---

<a name="line-261"></a>

```cpp
bxr_ecs_system_t bxr_ecs_system_define(bxr_ecs_t *ecs,
```

Define a new system in the ECS instance.

`ecs` is the ECS instance to define the system in.

`mask` is the system mask (a.k.a tags) to associate with the system.

`logic_cb` is the callback to call when the system is run.

`add_cb` is the callback to call when an entity is added to the system.

`remove_cb` is the callback to call when an entity is removed from the
system.

`udata` is the user data to associate with the system (can be NULL).

`return` the ID of the defined system, or an invalid ID if the system could
not be defined.
---

<a name="line-286"></a>

```cpp
void bxr_ecs_system_require_component(bxr_ecs_t *ecs,
```

Require a component for a system.

`ecs` is the ECS instance to modify.

`system` is the system to require the component for.

`component` is the component to require for the system.
---

<a name="line-299"></a>

```cpp
void bxr_ecs_system_exclude_component(bxr_ecs_t *ecs,
```

Exclude a component for a system.

`ecs` is the ECS instance to modify.

`system` is the system to exclude the component for.

`component` is the component to exclude for the system.
---

<a name="line-312"></a>

```cpp
void bxr_ecs_system_enable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
```

Enable a system, allowing it to be run when `bxr_ecs_system_run` or
`ecs_systems_run` is called.

`ecs` is the ECS instance to modify.

`sys` is the system to enable.
---

<a name="line-322"></a>

```cpp
void bxr_ecs_system_disable(bxr_ecs_t *ecs, bxr_ecs_system_t system)
```

Disable a system, preventing it from being run when `bxr_ecs_system_run` or
`ecs_systems_run` is called.

`ecs` is the ECS instance to modify.

`system` is the system to disable.
---

<a name="line-332"></a>

```cpp
void bxr_ecs_system_set_callback(bxr_ecs_t *ecs,
```

Set the callbacks for a system.

`ecs` is the ECS instance to modify.

`system` is the system to set the callbacks for.

`logic_cb` is the callback to call when the system is run.

`add_cb` is the callback to call when an entity is added to the system.

`remove_cb` is the callback to call when an entity is removed from the
system.
---

<a name="line-352"></a>

```cpp
void
```

Set the user data for a system.

`ecs` is the ECS instance to modify.

`system` is the system to set the user data for.

`udata` is the user data to associate with the system (can be NULL).
---

<a name="line-364"></a>

```cpp
void *bxr_ecs_system_get_udata(bxr_ecs_t *ecs, bxr_ecs_system_t system)
```

Get the user data for a system.

`ecs` is the ECS instance to query.

`system` is the system to get the user data for.

`return` the user data associated with the system, or NULL if no user data
is associated with the system.
---

<a name="line-376"></a>

```cpp
void bxr_ecs_system_set_mask(bxr_ecs_t *ecs,
```

Set the system mask (a.k.a tags).

`ecs` is the ECS instance to modify.

`system` is the system to set the mask for.

`mask` is the mask to set for the system.
---

<a name="line-389"></a>

```cpp
bxr_ecs_mask_t bxr_ecs_system_get_mask(bxr_ecs_t *ecs, bxr_ecs_system_t system)
```

Get the system mask (a.k.a tags).

`ecs` is the ECS instance to query.

`system` is the system to get the mask for.

`return` the mask associated with the system.
---

<a name="line-400"></a>

```cpp
size_t bxr_ecs_system_get_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t system)
```

Get the number of entities assigned to a system.
---

<a name="line-405"></a>

```cpp
int bxr_ecs_system_run(bxr_ecs_t *ecs,
```

Call a system logic on required component, but not exclueded ones.

`ecs` is the ECS instance to use.

`sys` is the system to run.

`mask` determines which systems run run based on a group/category.
---

<a name="line-418"></a>

```cpp
int bxr_ecs_systems_run(bxr_ecs_t *ecs, bxr_ecs_mask_t mask)
```

Call all system logic using `bxr_ecs_run_system`.

`ecs` is the ECS isntance tu use.

`mask` determines which systems run based on a group/category.
---
