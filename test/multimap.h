/**
 * @file multimap.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include "bxr.h"

bxr_multimap_t *multimap = NULL;

Uint32 audio_1 = 1;
Uint32 audio_2 = 2;
Uint32 audio_3 = 3;
Uint32 audio_4 = 4;
Uint32 audio_5 = 5;

static void
multimap_test_setup(void)
{
  multimap = bxr_multimap_create(16);
}

static void
multimap_test_teardown(void)
{
  bxr_multimap_destroy(multimap);
}

BXR_UNIT_CASE(case_multimap_push)
{
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_1));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_2));
  // Should not add a new value to the bucket since audio_2 already exist
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_2));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 1);

  Uint32 **ui_values = NULL;
  size_t ui_count    = bxr_multimap_get_values(multimap, "ui", ui_values);
  BXR_UNIT_ASSERT(ui_count == 2);
  BXR_FREE(ui_values);

  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_3));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_4));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_5));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 2);

  Uint32 **gameplay_values = NULL;
  size_t gameplay_count
      = bxr_multimap_get_values(multimap, "gameplay", gameplay_values);
  BXR_UNIT_ASSERT(gameplay_count == 3);
  BXR_FREE(gameplay_values);

  return true;
}

BXR_UNIT_CASE(case_multimap_grow)
{
  for (int i = 0; i < 100; i++) {
    char key[32];
    SDL_snprintf(key, sizeof(key), "key_%d", i);
    BXR_UNIT_ASSERT(bxr_multimap_add(multimap, key, (Uint32)i));
  }

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 100);

  return true;
}

BXR_UNIT_CASE(case_multimap_remove_value)
{
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_3));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_4));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_5));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 1);

  Uint32 **gameplay_values = NULL;
  size_t gameplay_count
      = bxr_multimap_get_values(multimap, "gameplay", gameplay_values);
  BXR_UNIT_ASSERT(gameplay_count == 3);
  BXR_FREE(gameplay_values);

  // remove

  BXR_UNIT_ASSERT(bxr_multimap_remove_value(multimap, "gameplay", audio_5));

  gameplay_count
      = bxr_multimap_get_values(multimap, "gameplay", gameplay_values);
  BXR_UNIT_ASSERT(gameplay_count == 2);
  BXR_FREE(gameplay_values);

  return true;
}

BXR_UNIT_CASE(case_multimap_remove)
{
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_1));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_2));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 1);

  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_3));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_4));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "gameplay", audio_5));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 2);

  BXR_UNIT_ASSERT(bxr_multimap_remove(multimap, "ui"));

  // remove again should fail
  BXR_UNIT_ASSERT(bxr_multimap_remove(multimap, "ui") == false);

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 1);

  return true;
}

BXR_UNIT_CASE(case_multimap_get_values)
{
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_1));
  BXR_UNIT_ASSERT(bxr_multimap_add(multimap, "ui", audio_2));

  BXR_UNIT_ASSERT(bxr_multimap_count(multimap) == 1);

  Uint32 **ui_values = NULL;
  size_t ui_count    = bxr_multimap_get_values(multimap, "ui", ui_values);
  BXR_UNIT_ASSERT(ui_count == 2);
  BXR_FREE(ui_values);

  return true;
}

BXR_UNIT_SUITE(suite_multimap)
{
  bxr_unit_setup(multimap_test_setup, multimap_test_teardown);

  BXR_UNIT_RUN_CASE(case_multimap_push);
  BXR_UNIT_RUN_CASE(case_multimap_grow);
  BXR_UNIT_RUN_CASE(case_multimap_remove_value);
  BXR_UNIT_RUN_CASE(case_multimap_remove);
  BXR_UNIT_RUN_CASE(case_multimap_get_values);
}

#endif // MULTIMAP_H_
