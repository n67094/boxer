/**
 * @file bxr_json.h
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

#ifndef BXR_JSON_H_
#define BXR_JSON_H_

#include <SDL3/SDL.h>
#include <physfs.h>

#define BXR_JSON_DEFAULT_DATA_CAPACITY 1024

typedef enum
{
  BXR_JSON_ERROR = 0,
  BXR_JSON_END,
  BXR_JSON_OBJECT,
  BXR_JSON_ARRAY,
  BXR_JSON_STRING,
  BXR_JSON_NUMBER,
  BXR_JSON_BOOL,
  BXR_JSON_NULL,
} bxr_jsonr_e;

typedef struct bxr_jsonr_s bxr_jsonr_t;

typedef struct bxr_jsonr_token_s
{
  bxr_jsonr_e type;

  char *start;
  char *end;

  int depht;
} bxr_jsonr_token_t;

bxr_jsonr_t *bxr_jsonr(const char *path);
void bxr_jsonr_destroy(bxr_jsonr_t *reader);

bxr_jsonr_token_t bxr_jsonr_read(bxr_jsonr_t *reader);

bool bxr_jsonr_iter_object(bxr_jsonr_t *reader,
                           bxr_jsonr_token_t *obj,
                           bxr_jsonr_token_t *key,
                           bxr_jsonr_token_t *value);

bool bxr_jsonr_iter_array(bxr_jsonr_t *reader,
                          bxr_jsonr_token_t *arr,
                          bxr_jsonr_token_t *value);

bool bxr_jsonr_eq_str(bxr_jsonr_token_t *key, const char *expected);
bool bxr_jsonr_eq_number(bxr_jsonr_token_t *key, float expected);
bool bxr_jsonr_eq_bool(bxr_jsonr_token_t *key, bool expected);

const char *bxr_jsonr_token_str(bxr_jsonr_token_t *token);
float bxr_jsonr_token_number(bxr_jsonr_token_t *token);
bool bxr_jsonr_token_bool(bxr_jsonr_token_t *token);

// ----------------------------------------------------------------------------

typedef struct bxr_jsonw_s bxr_jsonw_t;

bxr_jsonw_t *bxr_jsonw_make(void);
void bxr_jsonw_destroy(bxr_jsonw_t *json);

bool bxr_jsonw_object_begin(bxr_jsonw_t *json);
bool bxr_jsonw_object_end(bxr_jsonw_t *json);

bool bxr_jsonw_array_begin(bxr_jsonw_t *json);
bool bxr_jsonw_array_end(bxr_jsonw_t *json);

bool bxr_jsonw_key(bxr_jsonw_t *json, const char *key);
bool bxr_jsonw_str(bxr_jsonw_t *json, const char *value);
bool bxr_jsonw_number(bxr_jsonw_t *json, float number);
bool bxr_jsonw_bool(bxr_jsonw_t *json, bool value);

bool bxr_jsonw_key_str(bxr_jsonw_t *json, const char *key, const char *value);
bool bxr_jsonw_key_number(bxr_jsonw_t *json, const char *key, float number);
bool bxr_jsonw_key_bool(bxr_jsonw_t *json, const char *key, bool value);

bool bxr_jsonw_save(bxr_jsonw_t *json, const char *path);

#endif // BXR_JSON_H_
