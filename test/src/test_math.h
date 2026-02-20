/**
 * @file test_math.c
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_MATH_H_
#define TEST_MATH_H_

#include "engine.h"

ENGINE_UNIT_CASE(case_engine_clamp) { return true; }

ENGINE_UNIT_CASE(case_engine_sign) { return true; }

ENGINE_UNIT_SUITE(suite_math)
{
  ENGINE_UNIT_RUN_CASE(case_engine_clamp);
  ENGINE_UNIT_RUN_CASE(case_engine_sign);
}

#endif // TEST_MATH_H_
