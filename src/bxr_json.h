/**
 * File `bxr_json.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # JSON File Reader `jsonr` and Writer `jsonw`
 *
 * Support objects, arrays, strings, numbers and booleans.
 *
 * File extension doesn't matter, but recommended to use `.json`.
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

/**
 * ## Reader API
 */

/**
 * Load a JSON file from the given path and return a reader for it.
 *
 * `path` is the path to the JSON file to load. (c.f `bxr_io_read` for path
 * resolution).
 *
 * `return` a reader for the loaded JSON file, or NULL if the file could not be
 * loaded.
 */
bxr_jsonr_t *bxr_jsonr_make(const char *path);

/**
 * Destroy a JSON reader and free its resources.
 *
 * `reader` is the JSON reader to destroy.
 */
void bxr_jsonr_destroy(bxr_jsonr_t *reader);

/**
 * Read the next token from the JSON reader.
 *
 * `reader` is the JSON reader to read from.
 *
 * `return` the next token read from the JSON reader, or a token with type
 * `BXR_JSON_ERROR` if an error occurred while reading.
 */
bxr_jsonr_token_t bxr_jsonr_read(bxr_jsonr_t *reader);

/**
 * Iterate over the key-value pairs in a JSON object.
 *
 * `reader` is the JSON reader to read from.
 *
 * `obj` is the token representing the JSON object to iterate over. This token
 * should have been returned by a previous call to `bxr_jsonr_read`.
 *
 * `key` is an output parameter to store the token representing the current key
 * in the iteration.
 *
 * `value` is an output parameter to store the token representing the current
 * value in the iteration.
 *
 * `return` 1 if a key-value pair was successfully read, 0 if the end of the
 * object was reached, or -1 if an error occurred while reading.
 */
int bxr_jsonr_iter_object(bxr_jsonr_t *reader,
                          bxr_jsonr_token_t *obj,
                          bxr_jsonr_token_t *key,
                          bxr_jsonr_token_t *value);

/**
 * Iterate over the values in a JSON array.
 *
 * `reader` is the JSON reader to read from.
 *
 * `arr` is the token representing the JSON array to iterate over. This token
 * should have been returned by a previous call to `bxr_jsonr_read`.
 *
 * `value` is an output parameter to store the token representing the current
 * value in the iteration.
 *
 * `return` 1 if a value was successfully read, 0 if the end of the array was
 * reached, or -1 if an error occurred while reading.
 */
int bxr_jsonr_iter_array(bxr_jsonr_t *reader,
                         bxr_jsonr_token_t *arr,
                         bxr_jsonr_token_t *value);

/**
 * Compare a JSON token with an expected string value.
 *
 * `key` is the JSON token to compare. This token should have been returned by a
 * previous call to `bxr_jsonr_read` and should have type `BXR_JSON_STRING`.
 *
 * `expected` is the expected string value to compare against.
 *
 * `return` true if the JSON token's string value matches the expected string,
 * or false otherwise.
 */
bool bxr_jsonr_eq_str(bxr_jsonr_token_t *key, const char *expected);

/**
 * Compare a JSON token with an expected number value.
 *
 * `key` is the JSON token to compare. This token should have been returned by a
 * previous call to `bxr_jsonr_read` and should have type `BXR_JSON_NUMBER`.
 *
 * `expected` is the expected number value to compare against.
 *
 * `return` true if the JSON token's number value matches the expected number,
 * or false otherwise.
 */
bool bxr_jsonr_eq_number(bxr_jsonr_token_t *key, float expected);

/**
 * Compare a JSON token with an expected boolean value.
 *
 * `key` is the JSON token to compare. This token should have been returned by a
 * previous call to `bxr_jsonr_read` and should have type `BXR_JSON_BOOL`.
 *
 * `expected` is the expected boolean value to compare against.
 *
 * `return` true if the JSON token's boolean value matches the expected boolean,
 * or false otherwise.
 */
bool bxr_jsonr_eq_bool(bxr_jsonr_token_t *key, bool expected);

/**
 * Get the string value of a JSON token.
 *
 * `token` is the JSON token to get the string value from. This token should
 * have been returned by a previous call to `bxr_jsonr_read` and should have
 * type `BXR_JSON_STRING`.
 *
 * `return` a pointer to the string value of the JSON token, or NULL if the
 * token does not have a string value.
 */
const char *bxr_jsonr_token_str(bxr_jsonr_token_t *token);

/**
 * Get the number value of a JSON token.
 *
 * `token` is the JSON token to get the number value from. This token should
 * have been returned by a previous call to `bxr_jsonr_read` and should have
 * type `BXR_JSON_NUMBER`.
 *
 * `return` the number value of the JSON token, or 0.0 if the token does not
 * have a number value.
 */
float bxr_jsonr_token_number(bxr_jsonr_token_t *token);

/**
 * Get the boolean value of a JSON token.
 *
 * `token` is the JSON token to get the boolean value from. This token should
 * have been returned by a previous call to `bxr_jsonr_read` and should have
 * type `BXR_JSON_BOOL`.
 *
 * `return` the boolean value of the JSON token, or false if the token does not
 * have a boolean value.
 */
bool bxr_jsonr_token_bool(bxr_jsonr_token_t *token);

/**
 * ## Writer API
 */

typedef struct bxr_jsonw_s bxr_jsonw_t;

/**
 * Create a new JSON writer.
 *
 * `return` a pointer to the newly created JSON writer, or NULL if the writer
 * could not be created.
 */
bxr_jsonw_t *bxr_jsonw_make(void);

/**
 * Destroy a JSON writer and free its resources.
 *
 * `json` is the JSON writer to destroy.
 */
void bxr_jsonw_destroy(bxr_jsonw_t *json);

/**
 * Begin a new JSON object in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the object was successfully begun, or false if an error
 * occurred.
 */
bool bxr_jsonw_object_begin(bxr_jsonw_t *json);

/**
 * End the current JSON object in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the object was successfully ended, or false if an error
 * occurred.
 */
bool bxr_jsonw_object_end(bxr_jsonw_t *json);

/**
 * Begin a new JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the array was successfully begun, or false if an error
 * occurred.
 */
bool bxr_jsonw_array_begin(bxr_jsonw_t *json);

/**
 * End the current JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the array was successfully ended, or false if an error
 * occurred.
 */
bool bxr_jsonw_array_end(bxr_jsonw_t *json);

/**
 * Add a key-value pair with a string value to the current JSON object in the
 * writer.
 *
 * `json` is the JSON writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `value` is the string value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_jsonw_key(bxr_jsonw_t *json, const char *key);

/**
 * Add a string value to the current JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `value` is the string value to add to the array.
 *
 * `return` true if the value was successfully added, or false if an error
 * occurred.
 */
bool bxr_jsonw_str(bxr_jsonw_t *json, const char *value);

/**
 * Add a number value to the current JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `number` is the number value to add to the array.
 *
 * `return` true if the value was successfully added, or false if an error
 * occurred.
 */
bool bxr_jsonw_number(bxr_jsonw_t *json, float number);

/**
 * Add a boolean value to the current JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `value` is the boolean value to add to the array.
 *
 * `return` true if the value was successfully added, or false if an error
 * occurred.
 */
bool bxr_jsonw_bool(bxr_jsonw_t *json, bool value);

/**
 * Add a key-value pair with a string value to the current JSON object in the
 * writer.
 *
 * `json` is the JSON writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `value` is the string value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_jsonw_key_str(bxr_jsonw_t *json, const char *key, const char *value);

/**
 * Add a key-value pair with a number value to the current JSON object in the
 * writer.
 *
 * `json` is the JSON writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `number` is the number value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_jsonw_key_number(bxr_jsonw_t *json, const char *key, float number);

/**
 * Add a key-value pair with a boolean value to the current JSON object in the
 * writer.
 *
 * `json` is the JSON writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `value` is the boolean value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_jsonw_key_bool(bxr_jsonw_t *json, const char *key, bool value);

/**
 * Save the JSON data from the writer to a file at the given path.
 *
 * `json` is the JSON writer containing the data to save.
 *
 * `path` is the path to the file where the JSON data should be saved. (c.f
 * `bxr_io_write` for path resolution).
 *
 * `return` true if the JSON data was successfully saved, or false if an error
 * occurred.
 */
bool bxr_jsonw_save(bxr_jsonw_t *json, const char *path);

#endif // BXR_JSON_H_
