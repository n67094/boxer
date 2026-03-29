/**
 * @file test_atom.c
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_ATOM_H_
#define TEST_ATOM_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_atom_make)
{
  // Try to add 2 time the same string, str_1 and str_2 should point to the same
  // string.
  const char *str_1 = bxr_atom_make("Hello World!");
  const char *str_2 = bxr_atom_make("Hello World!");

  BXR_UNIT_ASSERT(str_1 == str_2);

  // Adding a different string
  const char *str_3 = bxr_atom_make("Another string.");

  BXR_UNIT_ASSERT(str_1 != str_3);

  return true;
}

BXR_UNIT_CASE(case_bxr_atom_length)
{
  const char *lorem_ipsum
      = "\tLorem ipsum dolor sit amet,\n consectetur adipiscing elit.\n";

  const char *str = bxr_atom_make(lorem_ipsum);
  size_t len      = bxr_atom_length(str);

  BXR_UNIT_ASSERT(SDL_strlen(lorem_ipsum) == len);

  return true;
}

BXR_UNIT_SUITE(suite_atom)
{
  BXR_UNIT_RUN_CASE(case_bxr_atom_make);
  BXR_UNIT_RUN_CASE(case_bxr_atom_length);
}

#endif // TEST_ATOM_H_
