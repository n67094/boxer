/**
 * File `ecs.c`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ECS_H_
#define ECS_H_

#include "bxr.h"

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
    .max_entities   = 1024,
    .max_components = 64,
    .max_systems    = 16,
  };

  ecs = bxr_ecs_create(&desc);
}

void
ecs_test_teardown(void)
{
  bxr_ecs_destroy(ecs);
}

BXR_UNIT_CASE(case_ecs_reset)
{
  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1 = bxr_ecs_system_define(ecs, 0, NULL, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c1));
  BXR_UNIT_ASSERT(bxr_ecs_entity_has_component(ecs, e1, c2));

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 1);

  bxr_ecs_reset(ecs);

  BXR_UNIT_ASSERT(bxr_ecs_system_get_entity_count(ecs, s1) == 0);

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

BXR_UNIT_CASE(case_ecs_exclude)
{
  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));

  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1 = bxr_ecs_system_define(
      ecs, 0, exclude_system, exclude_add_cb, exclude_remove_cb, &state1);
  bxr_ecs_system_exclude_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);

  bxr_ecs_system_t s2 = bxr_ecs_system_define(
      ecs, 0, exclude_system, exclude_add_cb, exclude_remove_cb, &state2);
  bxr_ecs_system_require_component(ecs, s2, c2);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);

  bxr_ecs_entity_t e2 = bxr_ecs_entity_create(ecs);
  bxr_ecs_entity_add_component(ecs, e2, c2, NULL);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);

  BXR_UNIT_ASSERT(state1.count == 1);
  BXR_UNIT_ASSERT(state1.entity == e2);
  BXR_UNIT_ASSERT(state1.add_count == 1);
  BXR_UNIT_ASSERT(state1.remove_count == 0);

  BXR_UNIT_ASSERT(state2.count == 2);
  BXR_UNIT_ASSERT(state2.entity == e1);
  BXR_UNIT_ASSERT(state2.add_count == 2);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));

  // Removing c1 from e1 causes it to be added to the system
  bxr_ecs_entity_remove_component(ecs, e1, c1);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);

  BXR_UNIT_ASSERT(state1.count == 2);
  BXR_UNIT_ASSERT(state1.entity == e2);
  BXR_UNIT_ASSERT(state1.add_count == 1);
  BXR_UNIT_ASSERT(state1.remove_count == 0);

  BXR_UNIT_ASSERT(state2.count == 2);
  BXR_UNIT_ASSERT(state2.entity == e1);
  BXR_UNIT_ASSERT(state2.add_count == 0);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  BXR_MEMSET(&state1, 0, sizeof(exclude_sys_state_t));
  BXR_MEMSET(&state2, 0, sizeof(exclude_sys_state_t));

  // Adding c1 to e2 causes it to be removed from the system
  bxr_ecs_entity_add_component(ecs, e2, c1, NULL);

  bxr_ecs_system_run(ecs, s1, 0);
  bxr_ecs_system_run(ecs, s2, 0);

  BXR_UNIT_ASSERT(state1.count == 1);
  BXR_UNIT_ASSERT(state1.entity == e1);
  BXR_UNIT_ASSERT(state1.add_count == 0);
  BXR_UNIT_ASSERT(state1.remove_count == 1);

  BXR_UNIT_ASSERT(state2.count == 2);
  BXR_UNIT_ASSERT(state2.entity == e1);
  BXR_UNIT_ASSERT(state2.add_count == 0);
  BXR_UNIT_ASSERT(state2.remove_count == 0);

  return true;
}

BXR_UNIT_SUITE(suite_ecs)
{
  bxr_unit_setup(ecs_test_setup, ecs_test_teardown);

  BXR_UNIT_RUN_CASE(case_ecs_reset);
  BXR_UNIT_RUN_CASE(case_ecs_exclude);
  /*
  BXR_UNIT_RUN_CASE(test_exclude_remove_system);
  BXR_UNIT_RUN_CASE(test_exclude_add_system);
  BXR_UNIT_RUN_CASE(test_constructor);
  BXR_UNIT_RUN_CASE(test_destructor_remove);
  BXR_UNIT_RUN_CASE(test_destructor_destroy);
  BXR_UNIT_RUN_CASE(test_create_destroy);
  BXR_UNIT_RUN_CASE(test_add_remove);
  BXR_UNIT_RUN_CASE(test_add_systems);
  BXR_UNIT_RUN_CASE(test_remove);
  BXR_UNIT_RUN_CASE(test_destroy);
  BXR_UNIT_RUN_CASE(test_destroy_system);
  BXR_UNIT_RUN_CASE(test_remove_system);
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
