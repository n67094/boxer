/**
 * File `bitset.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BITSET_H_
#define BITSET_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bitset)
{

  bxr_bitset_t *bitset = bxr_bitset_make(96);

  return true;
}

BXR_UNIT_SUITE(suite_bitset) { BXR_UNIT_RUN_CASE(case_bitset); }

#endif // BITSET_H_
