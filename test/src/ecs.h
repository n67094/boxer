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

BXR_UNIT_CASE(case_ecs) { return true; }

BXR_UNIT_SUITE(suite_ecs) { BXR_UNIT_RUN_CASE(case_ecs); }

#endif // ECS_H_
