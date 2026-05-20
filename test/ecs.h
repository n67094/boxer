/**
 * File `ecs.c`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ECS_H_
#define ECS_H_

#include "bxr.h"

#define MIN_ENTITIES 256
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

static void
ecs_test_setup(void)
{
  ecs = bxr_ecs_create(MIN_ENTITIES);
}

static void
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

  for (int i = 0; i < MAX_ENTITIES; i++) {
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

static int
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

static int
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

static void
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

static void
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

// Tests that removing components from an entity while running a system causes
// it to be removed from the system.
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

static bxr_ecs_system_t _s1;

static int
queue_add_system(bxr_ecs_t *ecs,
                 bxr_ecs_entity_t *entities,
                 size_t entity_count,
                 void *udata)
{
  (void)entities;
  (void)entity_count;
  (void)udata;

  for (size_t i = 0; i < MIN_ENTITIES + 256; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, _c1, NULL);
  }

  return bxr_ecs_system_get_entity_count(ecs, _s1);
};

// Tests that adding entities to a system while running it causes them to be
// added to the system (deferred via the add queue).
BXR_UNIT_CASE(case_ecs_queue_add)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  _s1 = bxr_ecs_system_define(ecs, 0, queue_add_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, _s1, _c1);

  size_t inner_count = bxr_ecs_system_run(ecs, _s1, 0);
  size_t outer_count = bxr_ecs_system_get_entity_count(ecs, _s1);

  BXR_UNIT_ASSERT(outer_count == inner_count + 256);

  return true;
}

static int
queue_remove_system(bxr_ecs_t *ecs,
                    bxr_ecs_entity_t *entities,
                    size_t entity_count,
                    void *udata)
{
  (void)entities;
  (void)entity_count;
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_remove_component(ecs, entities[i], _c1);
  }

  return bxr_ecs_system_get_entity_count(ecs, _s1);
};

// Tests that removing entities from a system while running it causes them to be
// removed from the system (deferred via the remove queue).
BXR_UNIT_CASE(case_ecs_queue_remove)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  _s1 = bxr_ecs_system_define(ecs, 0, queue_remove_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, _s1, _c1);

  for (size_t i = 0; i < MAX_ENTITIES; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, _c1, NULL);
  }

  size_t inner_count = bxr_ecs_system_run(ecs, _s1, 0);
  size_t outer_count = bxr_ecs_system_get_entity_count(ecs, _s1);

  BXR_UNIT_ASSERT(inner_count == MAX_ENTITIES);
  BXR_UNIT_ASSERT(outer_count == 0);

  return true;
}

static int
queue_destroy_system(bxr_ecs_t *ecs,
                     bxr_ecs_entity_t *entities,
                     size_t entity_count,
                     void *udata)
{
  (void)entities;
  (void)entity_count;
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {
    bxr_ecs_entity_destroy(ecs, entities[i]);
  }

  return bxr_ecs_system_get_entity_count(ecs, _s1);
};

// Tests that destroying entities from a system while running it causes them to
// be removed from the system (deferred via the remove queue).
BXR_UNIT_CASE(case_ecs_queue_destroy)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  _s1 = bxr_ecs_system_define(ecs, 0, queue_destroy_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, _s1, _c1);

  for (size_t i = 0; i < MAX_ENTITIES; i++) {
    bxr_ecs_entity_t e = bxr_ecs_entity_create(ecs);
    bxr_ecs_entity_add_component(ecs, e, _c1, NULL);
  }

  size_t inner_count = bxr_ecs_system_run(ecs, _s1, 0);
  size_t outer_count = bxr_ecs_system_get_entity_count(ecs, _s1);

  BXR_UNIT_ASSERT(inner_count == MAX_ENTITIES);
  BXR_UNIT_ASSERT(outer_count == 0);

  return true;
}

static int
disable_system(bxr_ecs_t *ecs,
               bxr_ecs_entity_t *entities,
               size_t entity_count,
               void *udata)
{
  (void)entities;
  (void)entity_count;
  (void)udata;

  for (size_t i = 0; i < entity_count; i++) {

    if (bxr_ecs_entity_has_component(ecs, entities[i], _c1)) {
      test_component_t *component
          = (test_component_t *)bxr_ecs_entity_get_component(
              ecs, entities[i], _c1);
      component->used = true;
    }
  }

  return 0;
};

// Tests that disabling a system causes it to not run and that enabling it again
// causes it to run.
BXR_UNIT_CASE(case_ecs_system_enable_disable)
{
  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1
      = bxr_ecs_system_define(ecs, 0, disable_system, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, _c1);

  bxr_ecs_entity_t e1    = bxr_ecs_entity_create(ecs);
  test_component_t *comp = bxr_ecs_entity_add_component(ecs, e1, _c1, NULL);

  // Default enable
  comp->used = false;
  bxr_ecs_system_run(ecs, s1, 0);
  BXR_UNIT_ASSERT(comp->used == true);

  // Explicit disable
  comp->used = false;
  bxr_ecs_system_disable(ecs, s1);
  bxr_ecs_system_run(ecs, s1, 0);
  BXR_UNIT_ASSERT(comp->used == false);

  // Re-enable
  bxr_ecs_system_enable(ecs, s1);
  bxr_ecs_system_run(ecs, s1, 0);
  BXR_UNIT_ASSERT(comp->used == true);

  return true;
}

static int
system_mask(bxr_ecs_t *ecs,
            bxr_ecs_entity_t *entities,
            size_t entity_count,
            void *udata)
{
  (void)ecs;
  (void)entities;
  (void)entity_count;

  bool *run = udata;
  *run      = true;

  return 0;
};

// Tests that a system only runs when the correct system mask is provided.
BXR_UNIT_CASE(case_ecs_system_mask)
{
  bool run = false;

  _s1 = bxr_ecs_system_define(
      ecs, (1 << 0) | (1 << 1), system_mask, NULL, NULL, &run);

  bxr_ecs_system_run(ecs, _s1, 0);
  BXR_UNIT_ASSERT(!run);

  bxr_ecs_system_run(ecs, _s1, (1 << 3));
  BXR_UNIT_ASSERT(!run);

  bxr_ecs_system_run(ecs, _s1, (1 << 1));
  BXR_UNIT_ASSERT(run);

  run = false;
  bxr_ecs_system_run(ecs, _s1, (1 << 0) | (1 << 1));
  BXR_UNIT_ASSERT(run);

  run = false;
  bxr_ecs_system_run(ecs, _s1, (bxr_ecs_mask_t)-1);
  BXR_UNIT_ASSERT(run);

  return true;
}

static bool added   = false;
static bool removed = false;

static void
on_add_cb(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *udata)
{
  (void)ecs;
  (void)entity;
  (void)udata;
  added = true;
}

static void
on_remove_cb(bxr_ecs_t *ecs, bxr_ecs_entity_t entity, void *udata)
{
  (void)ecs;
  (void)entity;
  (void)udata;
  removed = true;
}

// Tests that the add and remove callbacks are called when components are added
// and removed from entities.
BXR_UNIT_CASE(case_ecs_system_add_remove_cb)
{
  bxr_ecs_system_t s1 = bxr_ecs_system_define(
      ecs, 0, dummy_system, on_add_cb, on_remove_cb, NULL);

  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);

  bxr_ecs_system_run(ecs, s1, 0);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);
  bxr_ecs_entity_destroy(ecs, e1);

  BXR_UNIT_ASSERT(added == true);
  BXR_UNIT_ASSERT(removed == true);

  return true;
}

static int
ret_system(bxr_ecs_t *ecs,
           bxr_ecs_entity_t *entities,
           size_t entity_count,
           void *udata)
{
  (void)ecs;
  (void)entities;
  (void)entity_count;
  (void)udata;

  return 42;
};

static int
alt_ret_system(bxr_ecs_t *ecs,
               bxr_ecs_entity_t *entities,
               size_t entity_count,
               void *udata)
{
  (void)ecs;
  (void)entities;
  (void)entity_count;
  (void)udata;

  return 24;
};

// Tests that the system callback is called when a system is run.
BXR_UNIT_CASE(case_ecs_system_cb)
{
  added   = false;
  removed = false;

  _s1 = bxr_ecs_system_define(ecs, 0, ret_system, NULL, NULL, NULL);

  _c1 = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_require_component(ecs, _s1, _c1);

  int retval = bxr_ecs_system_run(ecs, _s1, 0);
  BXR_UNIT_ASSERT(retval == 42);

  bxr_ecs_system_set_callbacks(
      ecs, _s1, alt_ret_system, on_add_cb, on_remove_cb);
  retval = bxr_ecs_system_run(ecs, _s1, 0);
  BXR_UNIT_ASSERT(retval == 24);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, _c1, NULL);
  BXR_UNIT_ASSERT(added == true);

  bxr_ecs_entity_destroy(ecs, e1);
  BXR_UNIT_ASSERT(removed == true);

  return true;
}

// Tests that the system user data is passed correctly to the system callback.
BXR_UNIT_CASE(case_ecs_system_udata)
{
  int value = 42;
  _s1       = bxr_ecs_system_define(ecs, 0, ret_system, NULL, NULL, NULL);

  bxr_ecs_system_set_udata(ecs, _s1, &value);

  void *udata = bxr_ecs_system_get_udata(ecs, _s1);
  BXR_UNIT_ASSERT(udata == &value);
  BXR_UNIT_ASSERT(*(int *)udata == 42);

  int new_value = 24;
  bxr_ecs_system_set_udata(ecs, _s1, &new_value);
  udata = bxr_ecs_system_get_udata(ecs, _s1);
  BXR_UNIT_ASSERT(udata == &new_value);
  BXR_UNIT_ASSERT(*(int *)udata == 24);

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

  BXR_UNIT_RUN_CASE(case_ecs_queue_add);
  BXR_UNIT_RUN_CASE(case_ecs_queue_remove);
  BXR_UNIT_RUN_CASE(case_ecs_queue_destroy);

  BXR_UNIT_RUN_CASE(case_ecs_system_enable_disable);
  BXR_UNIT_RUN_CASE(case_ecs_system_mask);
  BXR_UNIT_RUN_CASE(case_ecs_system_add_remove_cb);
  BXR_UNIT_RUN_CASE(case_ecs_system_cb);
  BXR_UNIT_RUN_CASE(case_ecs_system_udata);
}

#endif // ECS_H_
