/**
 * File `ecs.c`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ECS_H_
#define ECS_H_

#include "bxr.h"

#define MAX_ENTITIES 1024
#define MAX_COMPONENTS 64
#define MAX_SYSTEMS 16

bxr_ecs_t *ecs = NULL;

static int
dummy_system(bxr_ecs_t *ecs,
             bxr_ecs_entity_t *entities,
             size_t entity_count,
             void *udata)
{
  (void)ecs;
  (void)entities;
  (void)entity_count;
  (void)udata;
  return 0;
}

void
ecs_test_setup(void)
{
  bxr_ecs_desc_t desc = {
    .max_entities   = MAX_ENTITIES,
    .max_components = MAX_COMPONENTS,
    .max_systems    = MAX_SYSTEMS,
  };

  ecs = bxr_ecs_create(&desc);
}

void
ecs_test_teardown(void)
{
  bxr_ecs_destroy(ecs);
}

// Tests that ECS reset works correctly.
BXR_UNIT_CASE(case_ecs_reset)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  for (int i = 0; i < 10; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, c1, NULL);
    bxr_ecs_entity_add_component(ecs, e, c2, NULL);
    BXR_UNIT_ASSERT(bxr_ecs_entity_is_ready(ecs, e));
  }

  bxr_ecs_reset(ecs);

  return true;
}

typedef struct
{
  bxr_ecs_entity_t entity;
  size_t count;
  size_t add_count;
  size_t remove_count;
} exclude_sys_state_t;

static int
exclude_system(bxr_ecs_t *ecs,
               bxr_ecs_entity_t *entities,
               size_t entity_count,
               void *udata)
{
  (void)ecs;
  (void)entity_count;

  exclude_sys_state_t *state = (exclude_sys_state_t *)udata;

  state->count = entity_count;

  if (entity_count > 0) {
    state->entity = entities[0];
  }

  return 0;
}

static void
exclude_add_cb(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *udata)
{
  (void)ecs;
  (void)entity;

  exclude_sys_state_t *state = (exclude_sys_state_t *)udata;

  state->add_count++;
}

static void
exclude_remove_cb(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *udata)
{
  (void)ecs;
  (void)entity;

  exclude_sys_state_t *state = (exclude_sys_state_t *)udata;

  state->remove_count++;
}

exclude_sys_state_t state1 = { 0 };
exclude_sys_state_t state2 = { 0 };
exclude_sys_state_t state3 = { 0 };

// Tests that excluding components from systems works correctly, and that adding
// and removing components causes entities to be added and removed from systems
// as expected.
//
// Once we remove/add a component from an entity, that entity could be eligible
// for other systems to be added to.
BXR_UNIT_CASE(case_ecs_system_exclude)
{
  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state3, 0, sizeof(exclude_sys_state_t));

  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c3
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1 = bxr_ecs_system_define(
      ecs, 0, exclude_system, exclude_add_cb, exclude_remove_cb, &state1);
  bxr_ecs_system_exclude_component(ecs, s1, c1);
  bxr_ecs_system_exclude_component(ecs, s1, c2);
  bxr_ecs_system_require_component(ecs, s1, c3);

  bxr_ecs_system_t s2 = bxr_ecs_system_define(
      ecs, 0, exclude_system, exclude_add_cb, exclude_remove_cb, &state2);
  bxr_ecs_system_exclude_component(ecs, s2, c1);
  bxr_ecs_system_require_component(ecs, s2, c2);
  bxr_ecs_system_require_component(ecs, s2, c3);

  bxr_ecs_system_t s3 = bxr_ecs_system_define(
      ecs, 0, exclude_system, exclude_add_cb, exclude_remove_cb, &state3);
  bxr_ecs_system_require_component(ecs, s3, c1);
  bxr_ecs_system_require_component(ecs, s3, c2);
  bxr_ecs_system_require_component(ecs, s3, c3);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, c3, NULL);

  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);
  bxr_ecs_entity_add_component(ecs, e2, c3, NULL);

  bxr_ecs_entity_t e3 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e3, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e3, c2, NULL);
  bxr_ecs_entity_add_component(ecs, e3, c3, NULL);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);
  bxr_ecs_system_run(ecs, s3, 0);

  BXR_UNIT_ASSERT(state1.count == 1);
  BXR_UNIT_ASSERT(state1.entity == e1);
  BXR_UNIT_ASSERT(state1.add_count == 1);
  BXR_UNIT_ASSERT(state1.remove_count == 0);

  BXR_UNIT_ASSERT(state2.count == 1);
  BXR_UNIT_ASSERT(state2.entity == e2);
  BXR_UNIT_ASSERT(state2.add_count == 1);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  BXR_UNIT_ASSERT(state3.count == 1);
  BXR_UNIT_ASSERT(state3.entity == e3);
  BXR_UNIT_ASSERT(state3.add_count == 1);
  BXR_UNIT_ASSERT(state3.remove_count == 0);

  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state3, 0, sizeof(exclude_sys_state_t));

  // Removing c1 from e3 causes it to be added to s2 and removed from s3
  bxr_ecs_entity_remove_component(ecs, e3, c1);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);
  bxr_ecs_system_run(ecs, s3, 0);

  BXR_UNIT_ASSERT(state1.count == 1);
  BXR_UNIT_ASSERT(state1.entity == e1);
  BXR_UNIT_ASSERT(state1.add_count == 0);
  BXR_UNIT_ASSERT(state1.remove_count == 0);

  BXR_UNIT_ASSERT(state2.count == 2);
  BXR_UNIT_ASSERT(state2.entity == e2);
  BXR_UNIT_ASSERT(state2.add_count == 1);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  BXR_UNIT_ASSERT(state3.count == 0);
  BXR_UNIT_ASSERT(state3.entity == 0);
  BXR_UNIT_ASSERT(state3.add_count == 0);
  BXR_UNIT_ASSERT(state3.remove_count == 1);

  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state3, 0, sizeof(exclude_sys_state_t));

  // Removing c2 from e2 causes it to be added to s1 and removed from s2
  bxr_ecs_entity_remove_component(ecs, e2, c2);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);
  bxr_ecs_system_run(ecs, s3, 0);

  BXR_UNIT_ASSERT(state1.count == 2);
  BXR_UNIT_ASSERT(state1.entity == e1);
  BXR_UNIT_ASSERT(state1.add_count == 1);
  BXR_UNIT_ASSERT(state1.remove_count == 0);

  BXR_UNIT_ASSERT(state2.count == 1);
  BXR_UNIT_ASSERT(state2.entity == e3);
  BXR_UNIT_ASSERT(state2.add_count == 0);
  BXR_UNIT_ASSERT(state2.remove_count == 1);

  BXR_UNIT_ASSERT(state3.count == 0);
  BXR_UNIT_ASSERT(state3.entity == 0);
  BXR_UNIT_ASSERT(state3.add_count == 0);
  BXR_UNIT_ASSERT(state3.remove_count == 0);

  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state3, 0, sizeof(exclude_sys_state_t));

  // Adding c2 back to e2 causes it to be added to s2 and removed from s1
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);
  bxr_ecs_system_run(ecs, s3, 0);

  BXR_UNIT_ASSERT(state1.count == 1);
  BXR_UNIT_ASSERT(state1.entity == e1);
  BXR_UNIT_ASSERT(state1.add_count == 0);
  BXR_UNIT_ASSERT(state1.remove_count == 1);

  BXR_UNIT_ASSERT(state2.count == 2);
  BXR_UNIT_ASSERT(state2.entity == e3);
  BXR_UNIT_ASSERT(state2.add_count == 1);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  BXR_UNIT_ASSERT(state3.count == 0);
  BXR_UNIT_ASSERT(state3.entity == 0);
  BXR_UNIT_ASSERT(state3.add_count == 0);
  BXR_UNIT_ASSERT(state3.remove_count == 0);

  return true;
}

bxr_ecs_component_t _c1;

int
remove_exclude_system(bxr_ecs_t *ecs,
                      bxr_ecs_entity_t *entities,
                      size_t entity_count,
                      void *udata)
{
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_remove_component(ecs, entities[i], _c1);
  }

  return 0;
}

// Tests that removing an excluded component from an entity causes it to be
// added to the system.
BXR_UNIT_CASE(case_ecs_system_exclude_remove)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, remove_exclude_system, NULL, NULL, NULL);

  bxr_ecs_system_exclude_component(ecs, s1, _c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_system_t s2
      = bxr_ecs_system_define(ecs, 0, remove_exclude_system, NULL, NULL, NULL);
  bxr_ecs_system_require_component(ecs, s2, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, _c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 2);

  bxr_ecs_system_run(ecs, s2, 0);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 2);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 2);

  return true;
}

int
add_exclude_system(bxr_ecs_t *ecs,
                   bxr_ecs_entity_t *entities,
                   size_t entity_count,
                   void *udata)
{
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_add_component(ecs, entities[i], _c1, NULL);
  }

  return 0;
}

// Tests that adding an excluded component to an entity causes it to be removed
// from the system.
BXR_UNIT_CASE(case_ecs_system_exclude_add)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, add_exclude_system, NULL, NULL, NULL);

  bxr_ecs_system_exclude_component(ecs, s1, _c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_system_t s2
      = bxr_ecs_system_define(ecs, 0, add_exclude_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s2, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, _c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 2);

  bxr_ecs_system_run(ecs, s2, 0);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 0);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 2);

  return true;
}

typedef struct
{
  bool used;
} test_component_t;

typedef struct
{
  bool used;
} test_args_t;

void
constructor(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *ptr, void *args)
{
  (void)ecs;
  (void)entity;
  (void)args;

  test_args_t *test_args = (test_args_t *)args;

  test_component_t *component = (test_component_t *)ptr;
  component->used             = test_args->used;
}

// Tests that the component constructor is called when a component is added to
// an entity.
BXR_UNIT_CASE(case_ecs_component_constructor)
{
  bxr_ecs_component_t c1 = bxr_ecs_component_define(
      ecs, sizeof(test_component_t), constructor, NULL);

  bxr_ecs_entity_t e1         = bxr_ecs_entity_create(ecs);
  test_component_t *component = bxr_ecs_entity_add_component(
      ecs, e1, c1, &(test_args_t){ .used = true });

  BXR_UNIT_ASSERT(component->used == true);

  return true;
}

void
destructor(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *ptr)
{
  (void)ecs;
  (void)entity;

  test_component_t *component = (test_component_t *)ptr;
  component->used             = false;
}

// Tests that the component destructor is called when an entity is removed.
BXR_UNIT_CASE(case_ecs_component_destructor_remove)
{
  bxr_ecs_component_t c1 = bxr_ecs_component_define(
      ecs, sizeof(test_component_t), NULL, destructor);

  bxr_ecs_entity_t e1         = bxr_ecs_entity_create(ecs);
  test_component_t *component = bxr_ecs_entity_add_component(
      ecs, e1, c1, &(test_args_t){ .used = true });

  bxr_ecs_entity_remove_component(ecs, e1, c1);

  BXR_UNIT_ASSERT(component->used == false);

  return true;
}

// Tests that the component destructor is called when an entity is destroyed.
BXR_UNIT_CASE(case_ecs_component_destructor_destroy)
{
  bxr_ecs_component_t c1 = bxr_ecs_component_define(
      ecs, sizeof(test_component_t), NULL, destructor);

  bxr_ecs_entity_t e1         = bxr_ecs_entity_create(ecs);
  test_component_t *component = bxr_ecs_entity_add_component(
      ecs, e1, c1, &(test_args_t){ .used = true });

  bxr_ecs_entity_destroy(ecs, e1);

  BXR_UNIT_ASSERT(component->used == false);

  return true;
}

// Tests that creating and destroying entities works correctly.
BXR_UNIT_CASE(case_ecs_entity_create_destroy)
{
  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);

  BXR_UNIT_ASSERT(bxr_ecs_entity_is_ready(ecs, e1));

  bxr_ecs_entity_destroy(ecs, e1);

  BXR_UNIT_ASSERT(!bxr_ecs_entity_is_ready(ecs, e1));

  return true;
}

// Tests that adding and removing components from an entity works correctly.
BXR_UNIT_CASE(case_ecs_entity_add_remove)
{
  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);

  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c2));

  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c2));

  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c2));

  bxr_ecs_entity_remove_component(ecs, e1, c1);

  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c2));

  bxr_ecs_entity_remove_component(ecs, e1, c2);

  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c2));

  return true;
}

// Tests that adding components to an entity causes it to be added to the
// correct systems.
BXR_UNIT_CASE(case_ecs_entity_add)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, dummy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);

  bxr_ecs_system_t s2
      = bxr_ecs_system_define(ecs, 0, dummy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s2, c1);
  bxr_ecs_system_require_component(ecs, s2, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 0);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 0);

  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 0);

  bxr_ecs_entity_add_component(ecs, e2, c1, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 2);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 0);

  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 2);
  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s2) == 1);

  return true;
}

// Tests that removing components from an entity causes it to be removed from
// the correct systems.
BXR_UNIT_CASE(case_ecs_entity_remove)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c3
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, dummy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);

  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c2));
  bxr_ecs_entity_remove_component(ecs, e1, c2);
  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e1, c2));

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 0);

  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(ecs, e2, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);
  bxr_ecs_entity_add_component(ecs, e2, c3, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);

  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e2, c3));
  bxr_ecs_entity_remove_component(ecs, e2, c3);
  BXR_UNIT_ASSERT(!bxr_ecs_entity_has_component(ecs, e2, c3));

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);

  return true;
}

static int
remove_system(bxr_ecs_t *ecs,
              bxr_ecs_entity_t *entities,
              size_t entity_count,
              void *udata)
{
  (void)entities;
  (void)entity_count;
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_remove_component(ecs, entities[i], _c1);

    if (bxr_ecs_entity_has_component(ecs, entities[i], _c1)) {
      return -1; // Component should have been removed
    }
  }

  return 0;
};

BXR_UNIT_CASE(case_ecs_entity_remove_system)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, remove_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, _c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  for (int i = 0; i < MAX_ENTITIES; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, _c1, NULL);
    bxr_ecs_entity_add_component(ecs, e, c2, NULL);
    BXR_UNIT_ASSERT(bxr_ecs_entity_is_ready(ecs, e));
  }

  int result = bxr_ecs_system_run(ecs, s1, 0);

  BXR_UNIT_ASSERT(result == 0);

  return true;
}

// Tests that destroying an entity causes it to be removed from systems and that
// it is no longer considered ready.
BXR_UNIT_CASE(case_ecs_entity_destroy)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, dummy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(ecs, e, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);

  bxr_ecs_entity_destroy(ecs, e);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 0);

  BXR_UNIT_ASSERT(!bxr_ecs_entity_is_ready(ecs, e));

  return true;
}

static int
destroy_system(bxr_ecs_t *ecs,
               bxr_ecs_entity_t *entities,
               size_t entity_count,
               void *udata)
{
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_destroy(ecs, entities[i]);

    if (bxr_ecs_entity_is_ready(ecs, entities[i])) {
      return -1; // Entity should have been destroyed
    }
  }

  return 0;
};

// Tests that destroying an entity causes it to be removed from systems while
// running.
BXR_UNIT_CASE(case_ecs_entity_destroy_system)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, destroy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  for (int i = 0; i < MAX_ENTITIES; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, c1, NULL);
    bxr_ecs_entity_add_component(ecs, e, c2, NULL);
    BXR_UNIT_ASSERT(bxr_ecs_entity_is_ready(ecs, e));
  }

  int result = bxr_ecs_system_run(ecs, s1, 0);

  BXR_UNIT_ASSERT(result == 0);

  return true;
}

BXR_UNIT_SUITE(suite_ecs)
{
  bxr_unit_setup(ecs_test_setup, ecs_test_teardown);

  BXR_UNIT_RUN_CASE(case_ecs_reset);

  BXR_UNIT_RUN_CASE(case_ecs_system_exclude);
  BXR_UNIT_RUN_CASE(case_ecs_system_exclude_remove);
  BXR_UNIT_RUN_CASE(case_ecs_system_exclude_add);

  BXR_UNIT_RUN_CASE(case_ecs_component_constructor);
  BXR_UNIT_RUN_CASE(case_ecs_component_destructor_remove);
  BXR_UNIT_RUN_CASE(case_ecs_component_destructor_destroy);

  BXR_UNIT_RUN_CASE(case_ecs_entity_create_destroy);
  BXR_UNIT_RUN_CASE(case_ecs_entity_add_remove);
  BXR_UNIT_RUN_CASE(case_ecs_entity_add);
  BXR_UNIT_RUN_CASE(case_ecs_entity_remove);
  BXR_UNIT_RUN_CASE(case_ecs_entity_remove_system);
  BXR_UNIT_RUN_CASE(case_ecs_entity_destroy);
  BXR_UNIT_RUN_CASE(case_ecs_entity_destroy_system);

  /*
  BXR_UNIT_RUN_CASE(test_queue_add);
  BXR_UNIT_RUN_CASE(test_queue_remove);
  BXR_UNIT_RUN_CASE(test_queue_destroy);
  BXR_UNIT_RUN_CASE(test_enable_disable);
  BXR_UNIT_RUN_CASE(test_system_mask);
  BXR_UNIT_RUN_CASE(test_add_remove_callbacks);
  BXR_UNIT_RUN_CASE(test_set_system_callbacks);
  BXR_UNIT_RUN_CASE(test_system_udata);
  BXR_UNIT_RUN_CASE(test_capacity_validation);
  */
}

#endif // ECS_H_
