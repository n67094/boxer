/**
 * @file test_ini.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_INI_H_
#define TEST_INI_H_

#include "engine.h"

ENGINE_UNIT_CASE(case_engine_ini_write)
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

  engine_iniw_t *ini = engine_iniw();
  ENGINE_UNIT_ASSERT(ini != NULL);

  ENGINE_UNIT_ASSERT(engine_iniw_section_begin(ini, "Window"));
  {
    ENGINE_UNIT_ASSERT(engine_iniw_key_str(ini, "title", "Test"));
    ENGINE_UNIT_ASSERT(engine_iniw_key_number(ini, "width", 800));
    ENGINE_UNIT_ASSERT(engine_iniw_key_number(ini, "height", 600));
  }
  ENGINE_UNIT_ASSERT(engine_iniw_section_end(ini));

  ENGINE_UNIT_ASSERT(engine_iniw_section_begin(ini, "Player"));
  {
    ENGINE_UNIT_ASSERT(engine_iniw_key_str(ini, "name", "nsix\ttest"));
  }
  ENGINE_UNIT_ASSERT(engine_iniw_section_end(ini));

  ENGINE_UNIT_ASSERT(engine_iniw_save(ini, "test_write.ini"));

  engine_iniw_destroy(ini);

  // read it back to verify

  engine_inir_t *read_ini = engine_inir("test_write.ini");

  const char *title = engine_inir_str(read_ini, "Window", "title");
  float width       = engine_inir_number(read_ini, "Window", "width");
  float height      = engine_inir_number(read_ini, "Window", "height");
  const char *name  = engine_inir_str(read_ini, "Player", "name");

  ENGINE_UNIT_ASSERT(SDL_strcmp(title, "Test") == 0);
  ENGINE_UNIT_ASSERT(width == 800.0f);
  ENGINE_UNIT_ASSERT(height == 600.0f);
  ENGINE_UNIT_ASSERT(SDL_strcmp(name, "nsix\ttest") == 0);

  return true;
}

ENGINE_UNIT_CASE(case_engine_ini_read)
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

  engine_io_write("test_read.ini", ini_content, SDL_strlen(ini_content), false);

  engine_inir_t *ini = engine_inir("test_read.ini");
  ENGINE_UNIT_ASSERT(ini != NULL);

  const char *title = engine_inir_str(ini, "Window", "title");
  float width       = engine_inir_number(ini, "Window", "width");
  float height      = engine_inir_number(ini, "Window", "height");
  const char *name  = engine_inir_str(ini, "Player", "name");
  float invalid     = engine_inir_number(ini, "Window", "invalid");
  float default_invalid
      = engine_inir_number_or_else(ini, "Window", "invalid", 500);

  ENGINE_UNIT_ASSERT(SDL_strcmp(title, "Test") == 0);
  ENGINE_UNIT_ASSERT(width == 800.0f);
  ENGINE_UNIT_ASSERT(height == 600.0f);
  ENGINE_UNIT_ASSERT(SDL_strcmp(name, "nsix\ttest") == 0);
  ENGINE_UNIT_ASSERT(invalid == 0.0f);
  ENGINE_UNIT_ASSERT(default_invalid == 500.0f);

  engine_inir_destroy(ini);

  return true;
}

ENGINE_UNIT_SUITE(suite_ini)
{
  // Writer cases
  ENGINE_UNIT_RUN_CASE(case_engine_ini_write);

  // Reader cases
  ENGINE_UNIT_RUN_CASE(case_engine_ini_read);
}

#endif // TEST_INI_H_
