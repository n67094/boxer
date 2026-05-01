/**
 * File `bitset.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BITSET_H_
#define BITSET_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bitset_is_zero)
{
  bxr_bitset_t *bitset = bxr_bitset_make(32);

  BXR_UNIT_ASSERT(bxr_bitset_is_zero(bitset));

  bxr_bitset_set(bitset, 0);

  BXR_UNIT_ASSERT(!bxr_bitset_is_zero(bitset));

  bxr_bitset_unset(bitset, 0);

  bxr_bitset_destroy(bitset);

  return true;
}

BXR_UNIT_CASE(case_bitset_true)
{
  bxr_bitset_t *bitset = bxr_bitset_make(32);

  BXR_UNIT_ASSERT(bxr_bitset_true(bitset) == false);

  bxr_bitset_set(bitset, 0);

  BXR_UNIT_ASSERT(bxr_bitset_true(bitset) == true);

  bxr_bitset_unset(bitset, 0);

  bxr_bitset_destroy(bitset);

  return true;
}

BXR_UNIT_CASE(case_bitset_test)
{
  bxr_bitset_t *bitset = bxr_bitset_make(32);

  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 0) == false);

  bxr_bitset_set(bitset, 0);

  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 0) == true);

  bxr_bitset_unset(bitset, 0);

  bxr_bitset_destroy(bitset);

  return true;
}

BXR_UNIT_CASE(case_bitset_set)
{
  bxr_bitset_t *bitset = bxr_bitset_make(96);

  bxr_bitset_set(bitset, 0);
  bxr_bitset_set(bitset, 31);

  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 0) == true);
  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 31) == true);

  bxr_bitset_destroy(bitset);

  return true;
}

BXR_UNIT_CASE(case_bitset_unset)
{
  bxr_bitset_t *bitset = bxr_bitset_make(96);

  bxr_bitset_set(bitset, 0);
  bxr_bitset_set(bitset, 31);

  bxr_bitset_unset(bitset, 0);

  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 0) == false);
  BXR_UNIT_ASSERT(bxr_bitset_test(bitset, 31) == true);

  bxr_bitset_destroy(bitset);

  return true;
}

BXR_UNIT_CASE(case_bitset_and)
{
  bxr_bitset_t *bitset_a = bxr_bitset_make(96);
  bxr_bitset_t *bitset_b = bxr_bitset_make(96);

  bxr_bitset_set(bitset_a, 0);
  bxr_bitset_set(bitset_a, 31);

  bxr_bitset_set(bitset_b, 31);
  bxr_bitset_set(bitset_b, 32);

  bxr_bitset_t *and_result = bxr_bitset_and(bitset_a, bitset_b);

  BXR_UNIT_ASSERT(bxr_bitset_test(and_result, 0) == false);
  BXR_UNIT_ASSERT(bxr_bitset_test(and_result, 31) == true);
  BXR_UNIT_ASSERT(bxr_bitset_test(and_result, 32) == false);
  return true;
}

BXR_UNIT_CASE(case_bitset_or)
{
  bxr_bitset_t *bitset_a = bxr_bitset_make(96);
  bxr_bitset_t *bitset_b = bxr_bitset_make(96);

  bxr_bitset_set(bitset_a, 0);
  bxr_bitset_set(bitset_a, 31);

  bxr_bitset_set(bitset_b, 31);
  bxr_bitset_set(bitset_b, 32);

  bxr_bitset_t *or_result = bxr_bitset_or(bitset_a, bitset_b);

  BXR_UNIT_ASSERT(bxr_bitset_test(or_result, 0) == true);
  BXR_UNIT_ASSERT(bxr_bitset_test(or_result, 31) == true);
  BXR_UNIT_ASSERT(bxr_bitset_test(or_result, 32) == true);

  return true;
}

BXR_UNIT_CASE(case_bitset_not)
{
  bxr_bitset_t *bitset = bxr_bitset_make(96);

  bxr_bitset_set(bitset, 0);
  bxr_bitset_set(bitset, 31);

  bxr_bitset_t *not_result = bxr_bitset_not(bitset);

  BXR_UNIT_ASSERT(bxr_bitset_test(not_result, 0) == false);
  BXR_UNIT_ASSERT(bxr_bitset_test(not_result, 31) == false);
  BXR_UNIT_ASSERT(bxr_bitset_test(not_result, 1) == true);
  BXR_UNIT_ASSERT(bxr_bitset_test(not_result, 30) == true);

  return true;
}

BXR_UNIT_CASE(case_bitset_equal)
{
  bxr_bitset_t *bitset_a = bxr_bitset_make(96);
  bxr_bitset_t *bitset_b = bxr_bitset_make(96);

  BXR_UNIT_ASSERT(bxr_bitset_equal(bitset_a, bitset_b) == true);

  bxr_bitset_set(bitset_a, 0);

  BXR_UNIT_ASSERT(bxr_bitset_equal(bitset_a, bitset_b) == false);

  bxr_bitset_set(bitset_b, 0);

  BXR_UNIT_ASSERT(bxr_bitset_equal(bitset_a, bitset_b) == true);

  return true;
}

BXR_UNIT_SUITE(suite_bitset)
{
  BXR_UNIT_RUN_CASE(case_bitset_is_zero);
  BXR_UNIT_RUN_CASE(case_bitset_true);
  BXR_UNIT_RUN_CASE(case_bitset_test);
  BXR_UNIT_RUN_CASE(case_bitset_set);
  BXR_UNIT_RUN_CASE(case_bitset_unset);
  BXR_UNIT_RUN_CASE(case_bitset_and);
  BXR_UNIT_RUN_CASE(case_bitset_or);
  BXR_UNIT_RUN_CASE(case_bitset_not);
  BXR_UNIT_RUN_CASE(case_bitset_equal);
}
#endif // BITSET_H_
