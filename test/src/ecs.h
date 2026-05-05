/**
 * File `ecs.c`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * TODO add test cases for ecs functions.
 */

#ifndef ECS_H_
#define ECS_H_

#include "bxr.h"

BXR_UNIT_CASE(case_ecs_reset)
{
  bxr_ecs_desc_t desc = {
    .max_entities   = 1024,
    .max_components = 64,
    .max_systems    = 16,
  };

  bxr_ecs_t *ecs = bxr_ecs_create(&desc);

  bxr_ecs_component_t c1
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c2
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);
  bxr_ecs_component_t c3
      = bxr_ecs_component_define(ecs, sizeof(bxr_ecs_component_t), NULL, NULL);

  bxr_ecs_system_t s1 = bxr_ecs_system_define(ecs, 0, NULL, NULL, NULL, NULL);

  bxr_ecs_system_require_component(ecs, s1, c1);
  bxr_ecs_system_require_component(ecs, s1, c2);
  bxr_ecs_system_exclude_component(ecs, s1, c3);

  bxr_ecs_entity_t e1 = bxr_ecs_entity_create(ecs);

  bxr_ecs_entity_add_component(ecs, e1, c1, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c2, NULL);
  bxr_ecs_entity_add_component(ecs, e1, c3, NULL);

  bxr_ecs_reset(ecs);

  bxr_ecs_destroy(ecs);

  return true;
}

BXR_UNIT_SUITE(suite_ecs)
{
  BXR_UNIT_RUN_CASE(case_ecs_reset);
  /*
  BXR_UNIT_RUN_CASE(test_exclude);
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
