/**
 * @file test_text.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_TEXT_H_
#define TEST_TEXT_H_

#include "engine.h"

static const char *_str_1 = "Hello, World!";

static const char *_str_2 = "{b=00FF00FF}{f=FF0000FF}Hello, World!{/b}{/f}";
static const char _str_2_expected_len
    = 13; // "Hello, World!" length without tags

static const char *_str_3
    = "{b=00FF00FF}{f=FF0000FF}Hello, World!{i=00}{/b}{/f}";
static const char _str_3_expected_len
    = 14; // "Hello, World!" length without tags + 1 icon

ENGINE_UNIT_CASE(case_engine_text)
{
  char *_str_1_no_tags = engine_text_without_tags(_str_1);
  ENGINE_UNIT_ASSERT(SDL_strcmp(_str_1, _str_1_no_tags) == 0);

  int _str_2_len = engine_text_length(_str_2);
  ENGINE_UNIT_ASSERT(_str_2_len == _str_2_expected_len);

  int _str_3_len = engine_text_length(_str_3);
  ENGINE_UNIT_ASSERT(_str_3_len == _str_3_expected_len);

  // TODO test measure

  // TODO test text wrape

  return true;
}

ENGINE_UNIT_SUITE(suite_text) { ENGINE_UNIT_RUN_CASE(case_engine_text); }

#endif // TEST_TEXT_H_
