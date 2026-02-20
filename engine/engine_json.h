/**
 * @file engine_json.h
 *
 * Thanks to rxi (sj.h) for the reader inspiration.
 *
 * @brief JSON file reader and writer.
 *
 * Support objects, arrays, strings, numbers and booleans.
 *
 * File extension doesn't matter, but recommended to use ".json".
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_JSON_H_
#define ENGINE_JSON_H_

#include <SDL3/SDL.h>
#include <physfs.h>

#define ENGINE_JSON_DEFAULT_DATA_CAPACITY 1024

typedef enum
{
  ENGINE_JSON_ERROR = 0,
  ENGINE_JSON_END,
  ENGINE_JSON_OBJECT,
  ENGINE_JSON_ARRAY,
  ENGINE_JSON_STRING,
  ENGINE_JSON_NUMBER,
  ENGINE_JSON_BOOL,
  ENGINE_JSON_NULL,
} engine_jsonr_e;

typedef struct engine_jsonr_s engine_jsonr_t;

typedef struct engine_jsonr_token_s
{
  engine_jsonr_e type;

  char *start;
  char *end;

  int depht;
} engine_jsonr_token_t;

engine_jsonr_t *engine_jsonr(const char *path);
void engine_jsonr_destroy(engine_jsonr_t *reader);

engine_jsonr_token_t engine_jsonr_read(engine_jsonr_t *reader);

bool engine_jsonr_iter_object(engine_jsonr_t *reader,
                              engine_jsonr_token_t *obj,
                              engine_jsonr_token_t *key,
                              engine_jsonr_token_t *value);

bool engine_jsonr_iter_array(engine_jsonr_t *reader,
                             engine_jsonr_token_t *arr,
                             engine_jsonr_token_t *value);

bool engine_jsonr_eq_str(engine_jsonr_token_t *key, const char *expected);
bool engine_jsonr_eq_number(engine_jsonr_token_t *key, float expected);
bool engine_jsonr_eq_bool(engine_jsonr_token_t *key, bool expected);

const char *engine_jsonr_token_str(engine_jsonr_token_t *token);
float engine_jsonr_token_number(engine_jsonr_token_t *token);
bool engine_jsonr_token_bool(engine_jsonr_token_t *token);

// ----------------------------------------------------------------------------

typedef struct engine_jsonw_s engine_jsonw_t;

engine_jsonw_t *engine_jsonw_make(void);
void engine_jsonw_destroy(engine_jsonw_t *json);

bool engine_jsonw_object_begin(engine_jsonw_t *json);
bool engine_jsonw_object_end(engine_jsonw_t *json);

bool engine_jsonw_array_begin(engine_jsonw_t *json);
bool engine_jsonw_array_end(engine_jsonw_t *json);

bool engine_jsonw_key(engine_jsonw_t *json, const char *key);
bool engine_jsonw_str(engine_jsonw_t *json, const char *value);
bool engine_jsonw_number(engine_jsonw_t *json, float number);
bool engine_jsonw_bool(engine_jsonw_t *json, bool value);

bool
engine_jsonw_key_str(engine_jsonw_t *json, const char *key, const char *value);
bool
engine_jsonw_key_number(engine_jsonw_t *json, const char *key, float number);
bool engine_jsonw_key_bool(engine_jsonw_t *json, const char *key, bool value);

bool engine_jsonw_save(engine_jsonw_t *json, const char *path);

#endif // ENGINE_JSON_H_
