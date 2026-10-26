/**
 * File `error.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ERROR_H_
#define ERROR_H_

#include "bxr.h"

BXR_UNIT_CASE(case_error_clear)
{
  bxr_set_error("Something failed");
  bxr_clear_error();

  BXR_UNIT_ASSERT(bxr_get_error() != NULL);
  BXR_UNIT_ASSERT(bxr_strcmp(bxr_get_error(), "") == 0);

  return true;
}

BXR_UNIT_CASE(case_error_set_get)
{
  BXR_UNIT_ASSERT(bxr_set_error("Failed to load %s (%d)", "level", 3) == false);
  BXR_UNIT_ASSERT(bxr_strcmp(bxr_get_error(), "Failed to load level (3)") == 0);

  return true;
}

BXR_UNIT_CASE(case_error_chain)
{
  bxr_set_error("inner");
  bxr_set_error("outer: %s", bxr_get_error());

  BXR_UNIT_ASSERT(bxr_strcmp(bxr_get_error(), "outer: inner") == 0);

  return true;
}

BXR_UNIT_CASE(case_error_truncate)
{
  char long_msg[BXR_ERROR_MAX_LENGTH * 2];
  SDL_memset(long_msg, 'a', sizeof(long_msg) - 1);
  long_msg[sizeof(long_msg) - 1] = '\0';

  bxr_set_error("%s", long_msg);

  BXR_UNIT_ASSERT(bxr_strlen(bxr_get_error()) == BXR_ERROR_MAX_LENGTH - 1);

  return true;
}

BXR_UNIT_CASE(case_error_asset_read)
{
  bxr_clear_error();

  Uint8 *data = bxr_asset_read("does_not_exist.txt", NULL);

  BXR_UNIT_ASSERT(data == NULL);
  BXR_UNIT_ASSERT(bxr_strstr(bxr_get_error(), "does_not_exist.txt") != NULL);

  return true;
}

BXR_UNIT_CASE(case_error_asset_mount)
{
  bxr_clear_error();

  BXR_UNIT_ASSERT(!bxr_asset_mount("/does/not/exist"));
  BXR_UNIT_ASSERT(bxr_strlen(bxr_get_error()) > 0);

  return true;
}

BXR_UNIT_SUITE(suite_error)
{
  BXR_UNIT_RUN_CASE(case_error_clear);
  BXR_UNIT_RUN_CASE(case_error_set_get);
  BXR_UNIT_RUN_CASE(case_error_chain);
  BXR_UNIT_RUN_CASE(case_error_truncate);
  BXR_UNIT_RUN_CASE(case_error_asset_read);
  BXR_UNIT_RUN_CASE(case_error_asset_mount);
}

#endif // ERROR_H_
