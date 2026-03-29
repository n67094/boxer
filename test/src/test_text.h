/**
 * @file test_text.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_TEXT_H_
#define TEST_TEXT_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_text_measure)
{
  const char *str = "Hello World!";

  // bxr_vec2_t size = bxr_text_measure(font, str);

  // BXR_UNIT_ASSERT(size.x > 0);
  // BXR_UNIT_ASSERT(size.y > 0);

  return true;
}

BXR_UNIT_SUITE(suite_text) { BXR_UNIT_RUN_CASE(case_bxr_text_measure); }

#endif // TEST_TEXT_H_
