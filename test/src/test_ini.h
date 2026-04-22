/**
 * @file test_ini.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_INI_H_
#define TEST_INI_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_ini_write)
{
  // Example INI file (test_write.ini):
  // ---------------------------------
  //
  // [Window]
  // title="Test"
  // width=800
  // height=600
  //
  // [Player]
  // name="nsix test"
  //

  bxr_iniw_t *ini = bxr_iniw_make();
  BXR_UNIT_ASSERT(ini != NULL);

  BXR_UNIT_ASSERT(bxr_iniw_section_begin(ini, "Window"));
  {
    BXR_UNIT_ASSERT(bxr_iniw_key_str(ini, "title", "Test"));
    BXR_UNIT_ASSERT(bxr_iniw_key_number(ini, "width", 800));
    BXR_UNIT_ASSERT(bxr_iniw_key_number(ini, "height", 600));
  }
  BXR_UNIT_ASSERT(bxr_iniw_section_end(ini));

  BXR_UNIT_ASSERT(bxr_iniw_section_begin(ini, "Player"));
  {
    BXR_UNIT_ASSERT(bxr_iniw_key_str(ini, "name", "nsix\ttest"));
  }
  BXR_UNIT_ASSERT(bxr_iniw_section_end(ini));

  BXR_UNIT_ASSERT(bxr_iniw_save(ini, "test_write.ini"));

  bxr_iniw_destroy(ini);

  // read it back to verify

  bxr_inir_t *read_ini = bxr_inir_make("test_write.ini");

  const char *title = bxr_inir_str(read_ini, "Window", "title");
  float width       = bxr_inir_number(read_ini, "Window", "width");
  float height      = bxr_inir_number(read_ini, "Window", "height");
  const char *name  = bxr_inir_str(read_ini, "Player", "name");

  BXR_UNIT_ASSERT(SDL_strcmp(title, "Test") == 0);
  BXR_UNIT_ASSERT(width == 800.0f);
  BXR_UNIT_ASSERT(height == 600.0f);
  BXR_UNIT_ASSERT(SDL_strcmp(name, "nsix\ttest") == 0);

  return true;
}

BXR_UNIT_CASE(case_bxr_ini_read)
{
  // Example INI file (test_read.ini):
  // ---------------------------------
  //
  // [Window] ; comment
  // title="Test"
  // width=800
  // height=600
  //
  // ; comment
  //
  // [Player]
  // name="nsix\ttest" ; Please note that the name contains a tab character
  // test= ; this will be ignored

  // Make sure the test_read.ini file exists with the above content before
  // running this test.

  const char *ini_content
      = "[Window] ; comment\n"
        "title=\"Test\"\n"
        "width=800\n"
        "height=600\n"
        "\n"
        "; comment\n"
        "\n"
        "[Player]\n"
        "name=\"nsix\ttest\" ; Please note that the name contains a "
        "tab character\n"
        "test= ; this will be ignored\n";

  bxr_io_write("test_read.ini", ini_content, SDL_strlen(ini_content), false);

  bxr_inir_t *ini = bxr_inir_make("test_read.ini");
  BXR_UNIT_ASSERT(ini != NULL);

  const char *title = bxr_inir_str(ini, "Window", "title");
  float width       = bxr_inir_number(ini, "Window", "width");
  float height      = bxr_inir_number(ini, "Window", "height");
  const char *name  = bxr_inir_str(ini, "Player", "name");
  float invalid     = bxr_inir_number(ini, "Window", "invalid");
  float default_invalid
      = bxr_inir_number_or_else(ini, "Window", "invalid", 500);

  BXR_UNIT_ASSERT(SDL_strcmp(title, "Test") == 0);
  BXR_UNIT_ASSERT(width == 800.0f);
  BXR_UNIT_ASSERT(height == 600.0f);
  BXR_UNIT_ASSERT(SDL_strcmp(name, "nsix\ttest") == 0);
  BXR_UNIT_ASSERT(invalid == 0.0f);
  BXR_UNIT_ASSERT(default_invalid == 500.0f);

  bxr_inir_destroy(ini);

  return true;
}

BXR_UNIT_SUITE(suite_ini)
{
  // Writer cases
  BXR_UNIT_RUN_CASE(case_bxr_ini_write);

  // Reader cases
  BXR_UNIT_RUN_CASE(case_bxr_ini_read);
}

#endif // TEST_INI_H_
