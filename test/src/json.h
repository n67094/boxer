/**
 * File `json.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef JSON_H_
#define JSON_H_

#include "bxr.h"

BXR_UNIT_CASE(case_bxr_json_write)
{
  bxr_json_writer_t *json = bxr_json_make_writer();

  BXR_UNIT_ASSERT(bxr_json_writer_object_begin(json));
  {
    BXR_UNIT_ASSERT(bxr_json_write_key_str(json, "name", "nsix"));
    BXR_UNIT_ASSERT(bxr_json_write_key_number(json, "id", 123));
    BXR_UNIT_ASSERT(bxr_json_write_key_bool(json, "is_human", false));

    // Array of strings
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "items"));
    BXR_UNIT_ASSERT(bxr_json_writer_array_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_write_str(json, "item1"));
      BXR_UNIT_ASSERT(bxr_json_write_str(json, "item2"));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_array_end(json));

    // Array of numbers
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "scores"));
    BXR_UNIT_ASSERT(bxr_json_writer_array_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_write_number(json, 10));
      BXR_UNIT_ASSERT(bxr_json_write_number(json, 20));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_array_end(json));

    // Array of booleans
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "flags"));
    BXR_UNIT_ASSERT(bxr_json_writer_array_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_write_bool(json, true));
      BXR_UNIT_ASSERT(bxr_json_write_bool(json, false));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_array_end(json));

    // Array of mixed values
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "mixed"));
    BXR_UNIT_ASSERT(bxr_json_writer_array_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_write_str(json, "string"));
      BXR_UNIT_ASSERT(bxr_json_write_number(json, 42.24f));
      BXR_UNIT_ASSERT(bxr_json_write_bool(json, true));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_array_end(json));

    // Array of objects
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "players"));
    BXR_UNIT_ASSERT(bxr_json_writer_array_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_writer_object_begin(json));
      {
        BXR_UNIT_ASSERT(bxr_json_write_key_str(json, "name", "Alice"));
        BXR_UNIT_ASSERT(bxr_json_write_key_number(json, "score", 100));
      }
      BXR_UNIT_ASSERT(bxr_json_writer_object_end(json));

      BXR_UNIT_ASSERT(bxr_json_writer_object_begin(json));
      {
        BXR_UNIT_ASSERT(bxr_json_write_key_str(json, "name", "Bob"));
        BXR_UNIT_ASSERT(bxr_json_write_key_number(json, "score", 150));
      }
      BXR_UNIT_ASSERT(bxr_json_writer_object_end(json));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_array_end(json));

    // Object with string values
    BXR_UNIT_ASSERT(bxr_json_write_key(json, "attributes"));
    BXR_UNIT_ASSERT(bxr_json_writer_object_begin(json));
    {
      BXR_UNIT_ASSERT(bxr_json_write_key_str(json, "strength", "high"));
      BXR_UNIT_ASSERT(bxr_json_write_key_str(json, "agility", "medium"));
    }
    BXR_UNIT_ASSERT(bxr_json_writer_object_end(json));
  }
  BXR_UNIT_ASSERT(bxr_json_writer_object_end(json));

  BXR_UNIT_ASSERT(bxr_json_writer_save(json, "test.json"));

  bxr_json_destroy_writer(json);

  return true;
}

BXR_UNIT_CASE(case_bxr_json_read)
{
  // Please note that here bxr_jsonr_eq_* could be bxr_jsonr_token_str
  // functions, but using eq functions makes the test more readable and also
  // tests the eq functions themselves.

  bxr_json_reader_t *json = bxr_json_make_reader("test.json");
  bxr_json_token_t token  = bxr_json_read_token(json);

  bxr_json_token_t key, value;
  while (bxr_json_reader_iter_object(json, &token, &key, &value) == 1) {
    if (bxr_json_reader_eq_str(&key, "name")) {
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&value, "nsix"));
    } else if (bxr_json_reader_eq_str(&key, "id")) {
      BXR_UNIT_ASSERT(bxr_json_reader_eq_number(&value, 123));
    } else if (bxr_json_reader_eq_str(&key, "is_human")) {
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&value, "false"));
    } else if (bxr_json_reader_eq_str(&key, "items")) {
      bxr_json_token_t item;
      int item_index = 0;
      while (bxr_json_reader_iter_array(json, &value, &item)) {
        if (item_index == 0) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&item, "item1"));
        } else if (item_index == 1) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&item, "item2"));
        }
        item_index++;
      }
      BXR_UNIT_ASSERT(item_index == 2);
    } else if (bxr_json_reader_eq_str(&key, "scores")) {
      bxr_json_token_t score;
      int score_index = 0;
      while (bxr_json_reader_iter_array(json, &value, &score) == 1) {
        if (score_index == 0) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(
              &score, "10")); // intentionally using eq_str
        } else if (score_index == 1) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_number(&score, 20));
        }
        score_index++;
      }
      BXR_UNIT_ASSERT(score_index == 2);
    } else if (bxr_json_reader_eq_str(&key, "flags")) {
      bxr_json_token_t flag;
      int flag_index = 0;
      while (bxr_json_reader_iter_array(json, &value, &flag) == 1) {
        if (flag_index == 0) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(
              &flag, "true")); // intentionally using eq_str
        } else if (flag_index == 1) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_bool(&flag, false));
        }
        flag_index++;
      }
      BXR_UNIT_ASSERT(flag_index == 2);
    } else if (bxr_json_reader_eq_str(&key, "mixed")) {
      bxr_json_token_t mixed_value;
      int mixed_index = 0;
      while (bxr_json_reader_iter_array(json, &value, &mixed_value) == 1) {
        if (mixed_index == 0) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&mixed_value, "string"));
        } else if (mixed_index == 1) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_number(&mixed_value, 42.24f));
        } else if (mixed_index == 2) {
          BXR_UNIT_ASSERT(bxr_json_reader_eq_bool(&mixed_value, true));
        }
        mixed_index++;
      }
      BXR_UNIT_ASSERT(mixed_index == 3);
    } else if (bxr_json_reader_eq_str(&key, "players")) {
      bxr_json_token_t player;
      int player_index = 0;
      while (bxr_json_reader_iter_array(json, &value, &player) == 1) {
        bxr_json_token_t player_key, player_value;
        if (player_index == 0) {
          BXR_UNIT_ASSERT(bxr_json_reader_iter_object(
              json, &player, &player_key, &player_value));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_key, "name"));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_value, "Alice"));
          BXR_UNIT_ASSERT(bxr_json_reader_iter_object(
              json, &player, &player_key, &player_value));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_key, "score"));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_number(&player_value, 100));
        } else if (player_index == 1) {
          BXR_UNIT_ASSERT(bxr_json_reader_iter_object(
              json, &player, &player_key, &player_value));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_key, "name"));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_value, "Bob"));
          BXR_UNIT_ASSERT(bxr_json_reader_iter_object(
              json, &player, &player_key, &player_value));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&player_key, "score"));
          BXR_UNIT_ASSERT(bxr_json_reader_eq_number(&player_value, 150));
        }
        player_index++;
      }
      BXR_UNIT_ASSERT(player_index == 2);
    } else if (bxr_json_reader_eq_str(&key, "attributes")) {
      bxr_json_token_t attr_key, attr_value;
      BXR_UNIT_ASSERT(
          bxr_json_reader_iter_object(json, &value, &attr_key, &attr_value));
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&attr_key, "strength"));
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&attr_value, "high"));
      BXR_UNIT_ASSERT(
          bxr_json_reader_iter_object(json, &value, &attr_key, &attr_value));
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&attr_key, "agility"));
      BXR_UNIT_ASSERT(bxr_json_reader_eq_str(&attr_value, "medium"));
    }
  }

  return true;
}

BXR_UNIT_SUITE(suite_json)
{
  BXR_UNIT_RUN_CASE(case_bxr_json_write);
  BXR_UNIT_RUN_CASE(case_bxr_json_read);
}

#endif // JSON_H_
