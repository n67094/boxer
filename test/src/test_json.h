/**
 * @file test_json.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TEST_JSON_H_
#define TEST_JSON_H_

#include "engine.h"

ENGINE_UNIT_CASE(case_engine_json_write)
{
  engine_jsonw_t *json = engine_jsonw_make();

  ENGINE_UNIT_ASSERT(engine_jsonw_object_begin(json));
  {
    ENGINE_UNIT_ASSERT(engine_jsonw_key_str(json, "name", "nsix"));
    ENGINE_UNIT_ASSERT(engine_jsonw_key_number(json, "id", 123));
    ENGINE_UNIT_ASSERT(engine_jsonw_key_bool(json, "is_human", false));

    // Array of strings
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "items"));
    ENGINE_UNIT_ASSERT(engine_jsonw_array_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_str(json, "item1"));
      ENGINE_UNIT_ASSERT(engine_jsonw_str(json, "item2"));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_array_end(json));

    // Array of numbers
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "scores"));
    ENGINE_UNIT_ASSERT(engine_jsonw_array_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_number(json, 10));
      ENGINE_UNIT_ASSERT(engine_jsonw_number(json, 20));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_array_end(json));

    // Array of booleans
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "flags"));
    ENGINE_UNIT_ASSERT(engine_jsonw_array_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_bool(json, true));
      ENGINE_UNIT_ASSERT(engine_jsonw_bool(json, false));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_array_end(json));

    // Array of mixed values
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "mixed"));
    ENGINE_UNIT_ASSERT(engine_jsonw_array_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_str(json, "string"));
      ENGINE_UNIT_ASSERT(engine_jsonw_number(json, 42.24f));
      ENGINE_UNIT_ASSERT(engine_jsonw_bool(json, true));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_array_end(json));

    // Array of objects
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "players"));
    ENGINE_UNIT_ASSERT(engine_jsonw_array_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_object_begin(json));
      {
        ENGINE_UNIT_ASSERT(engine_jsonw_key_str(json, "name", "Alice"));
        ENGINE_UNIT_ASSERT(engine_jsonw_key_number(json, "score", 100));
      }
      ENGINE_UNIT_ASSERT(engine_jsonw_object_end(json));

      ENGINE_UNIT_ASSERT(engine_jsonw_object_begin(json));
      {
        ENGINE_UNIT_ASSERT(engine_jsonw_key_str(json, "name", "Bob"));
        ENGINE_UNIT_ASSERT(engine_jsonw_key_number(json, "score", 150));
      }
      ENGINE_UNIT_ASSERT(engine_jsonw_object_end(json));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_array_end(json));

    // Object with string values
    ENGINE_UNIT_ASSERT(engine_jsonw_key(json, "attributes"));
    ENGINE_UNIT_ASSERT(engine_jsonw_object_begin(json));
    {
      ENGINE_UNIT_ASSERT(engine_jsonw_key_str(json, "strength", "high"));
      ENGINE_UNIT_ASSERT(engine_jsonw_key_str(json, "agility", "medium"));
    }
    ENGINE_UNIT_ASSERT(engine_jsonw_object_end(json));
  }
  ENGINE_UNIT_ASSERT(engine_jsonw_object_end(json));

  ENGINE_UNIT_ASSERT(engine_jsonw_save(json, "test.json"));

  engine_jsonw_destroy(json);

  return true;
}

ENGINE_UNIT_CASE(case_engine_json_read)
{
  // Please note that here engine_jsonr_eq_* could be engine_jsonr_token_str
  // functions, but using eq functions makes the test more readable and also
  // tests the eq functions themselves.

  engine_jsonr_t *json       = engine_jsonr("test.json");
  engine_jsonr_token_t token = engine_jsonr_read(json);

  engine_jsonr_token_t key, value;
  while (engine_jsonr_iter_object(json, &token, &key, &value)) {
    if (engine_jsonr_eq_str(&key, "name")) {
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&value, "nsix"));
    } else if (engine_jsonr_eq_str(&key, "id")) {
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_number(&value, 123));
    } else if (engine_jsonr_eq_str(&key, "is_human")) {
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&value, "false"));
    } else if (engine_jsonr_eq_str(&key, "items")) {
      engine_jsonr_token_t item;
      int item_index = 0;
      while (engine_jsonr_iter_array(json, &value, &item)) {
        if (item_index == 0) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&item, "item1"));
        } else if (item_index == 1) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&item, "item2"));
        }
        item_index++;
      }
      ENGINE_UNIT_ASSERT(item_index == 2);
    } else if (engine_jsonr_eq_str(&key, "scores")) {
      engine_jsonr_token_t score;
      int score_index = 0;
      while (engine_jsonr_iter_array(json, &value, &score)) {
        if (score_index == 0) {
          ENGINE_UNIT_ASSERT(
              engine_jsonr_eq_str(&score, "10")); // intentionally using eq_str
        } else if (score_index == 1) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_number(&score, 20));
        }
        score_index++;
      }
      ENGINE_UNIT_ASSERT(score_index == 2);
    } else if (engine_jsonr_eq_str(&key, "flags")) {
      engine_jsonr_token_t flag;
      int flag_index = 0;
      while (engine_jsonr_iter_array(json, &value, &flag)) {
        if (flag_index == 0) {
          ENGINE_UNIT_ASSERT(
              engine_jsonr_eq_str(&flag, "true")); // intentionally using eq_str
        } else if (flag_index == 1) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_bool(&flag, false));
        }
        flag_index++;
      }
      ENGINE_UNIT_ASSERT(flag_index == 2);
    } else if (engine_jsonr_eq_str(&key, "mixed")) {
      engine_jsonr_token_t mixed_value;
      int mixed_index = 0;
      while (engine_jsonr_iter_array(json, &value, &mixed_value)) {
        if (mixed_index == 0) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&mixed_value, "string"));
        } else if (mixed_index == 1) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_number(&mixed_value, 42.24f));
        } else if (mixed_index == 2) {
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_bool(&mixed_value, true));
        }
        mixed_index++;
      }
      ENGINE_UNIT_ASSERT(mixed_index == 3);
    } else if (engine_jsonr_eq_str(&key, "players")) {
      engine_jsonr_token_t player;
      int player_index = 0;
      while (engine_jsonr_iter_array(json, &value, &player)) {
        engine_jsonr_token_t player_key, player_value;
        if (player_index == 0) {
          ENGINE_UNIT_ASSERT(engine_jsonr_iter_object(
              json, &player, &player_key, &player_value));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_key, "name"));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_value, "Alice"));
          ENGINE_UNIT_ASSERT(engine_jsonr_iter_object(
              json, &player, &player_key, &player_value));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_key, "score"));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_number(&player_value, 100));
        } else if (player_index == 1) {
          ENGINE_UNIT_ASSERT(engine_jsonr_iter_object(
              json, &player, &player_key, &player_value));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_key, "name"));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_value, "Bob"));
          ENGINE_UNIT_ASSERT(engine_jsonr_iter_object(
              json, &player, &player_key, &player_value));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&player_key, "score"));
          ENGINE_UNIT_ASSERT(engine_jsonr_eq_number(&player_value, 150));
        }
        player_index++;
      }
      ENGINE_UNIT_ASSERT(player_index == 2);
    } else if (engine_jsonr_eq_str(&key, "attributes")) {
      engine_jsonr_token_t attr_key, attr_value;
      ENGINE_UNIT_ASSERT(
          engine_jsonr_iter_object(json, &value, &attr_key, &attr_value));
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&attr_key, "strength"));
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&attr_value, "high"));
      ENGINE_UNIT_ASSERT(
          engine_jsonr_iter_object(json, &value, &attr_key, &attr_value));
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&attr_key, "agility"));
      ENGINE_UNIT_ASSERT(engine_jsonr_eq_str(&attr_value, "medium"));
    }
  }

  return true;
}

ENGINE_UNIT_SUITE(suite_json)
{
  // Writer cases
  ENGINE_UNIT_RUN_CASE(case_engine_json_write);

  // Reader cases
  ENGINE_UNIT_RUN_CASE(case_engine_json_read);
}

#endif // TEST_JSON_H_
