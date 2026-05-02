/**
 * File `sparse_et.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef SPARSE_SET_H_
#define SPARSE_SET_H_

#include "bxr.h"

BXR_UNIT_CASE(case_sparse_set_insert)
{
  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));

  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 1, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 2, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 3, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 4, NULL));

  bxr_sparse_set_destroy(set);

  return true;
}

BXR_UNIT_CASE(case_sparse_set_remove)
{

  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));

  BXR_UNIT_ASSERT(bxr_sparse_set_remove(set, 2));
  BXR_UNIT_ASSERT(!bxr_sparse_set_contains(set, 2, NULL));

  BXR_UNIT_ASSERT(!bxr_sparse_set_remove(set, 5));

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));

  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 1, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 2, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 3, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 4, NULL));

  bxr_sparse_set_destroy(set);

  return true;
}

// Make the sparse set double it's size
BXR_UNIT_CASE(case_sparse_set_double)
{

  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 5));

  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 1, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 2, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 3, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 4, NULL));
  BXR_UNIT_ASSERT(bxr_sparse_set_contains(set, 5, NULL));

  bxr_sparse_set_destroy(set);

  return true;
}

BXR_UNIT_CASE(case_sprase_set_capacity)
{
  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_capacity(set) == 4);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 5));

  BXR_UNIT_ASSERT(bxr_sparse_set_capacity(set) == 8);

  bxr_sparse_set_destroy(set);

  return true;
}

BXR_UNIT_CASE(case_sparse_set_count)
{
  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_count(set) == 0);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));

  BXR_UNIT_ASSERT(bxr_sparse_set_count(set) == 4);

  BXR_UNIT_ASSERT(bxr_sparse_set_remove(set, 2));

  BXR_UNIT_ASSERT(bxr_sparse_set_count(set) == 3);

  bxr_sparse_set_destroy(set);

  return true;
}

BXR_UNIT_CASE(case_sparse_set_clear)
{
  bxr_sparse_set_t *set = bxr_sparse_set_create(4);

  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 1));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 2));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 3));
  BXR_UNIT_ASSERT(bxr_sparse_set_insert(set, 4));

  bxr_sparse_set_clear(set);

  BXR_UNIT_ASSERT(bxr_sparse_set_count(set) == 0);

  bxr_sparse_set_destroy(set);

  return true;
}

BXR_UNIT_SUITE(suite_sparse_set)
{
  BXR_UNIT_RUN_CASE(case_sparse_set_insert);
  BXR_UNIT_RUN_CASE(case_sparse_set_remove);
  BXR_UNIT_RUN_CASE(case_sparse_set_double);
  BXR_UNIT_RUN_CASE(case_sprase_set_capacity);
  BXR_UNIT_RUN_CASE(case_sparse_set_count);
  BXR_UNIT_RUN_CASE(case_sparse_set_clear);
}

#endif // SPARSE_SET_H_
