/**
 * @file test_math.c
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_MATH_H_
#define TEST_MATH_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_clamp) { return true; }

BXR_UNIT_CASE(case_bxr_sign) { return true; }

BXR_UNIT_SUITE(suite_math)
{
  BXR_UNIT_RUN_CASE(case_bxr_clamp);
  BXR_UNIT_RUN_CASE(case_bxr_sign);
}

#endif // TEST_MATH_H_
