/**
 * File `math.c`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * TODO add test cases for math functions.
 */

#ifndef MATH_H_
#define MATH_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_clamp) { return true; }

BXR_UNIT_CASE(case_bxr_sign) { return true; }

BXR_UNIT_SUITE(suite_math)
{
  BXR_UNIT_RUN_CASE(case_bxr_clamp);
  BXR_UNIT_RUN_CASE(case_bxr_sign);
}

#endif // MATH_H_
