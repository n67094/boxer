/**
 * File `bxr_json.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # JSON File Reader and Writer
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

typedef struct bxr_json_reader_s bxr_json_reader_t;

typedef struct bxr_json_token_s
{
  bxr_jsonr_e type;

  char *start;
  char *end;

  int depht;
} bxr_json_token_t;

/**
 * ## Reader API
 */

/**
 * Load a JSON file from the given data and return a reader for it.
 *
 * `data` is a pointer to the JSON data to load.
 *
 * `length` is the length of the JSON data in bytes.
 *
 * `return` a reader for the loaded JSON file, or NULL if an error occurred. Use
 * `bxr_error_get` to get more information about the error.
 *
 * The caller is responsible for destroying the returned reader using
 * `bxr_json_destroy_reader` when it is no longer needed.
 */
bxr_json_reader_t *bxr_json_create_reader(const Uint8 *data, size_t length);

/**
 * Destroy a JSON reader and free its resources.
 *
 * `reader` is the JSON reader to destroy.
 */
void bxr_json_destroy_reader(bxr_json_reader_t *reader);

/**
 * Read the next token from the JSON reader.
 *
 * `reader` is the JSON reader to read from.
 *
 * `return` the next token read from the JSON reader, or a token with type
 * `BXR_JSON_ERROR` if an error occurred while reading. Use `bxr_error_get` to
 * get more information about the error.
 *
 * The caller should not modify or free these pointers, as they are owned by the
 * JSON reader.
 */
bxr_json_token_t bxr_json_read_token(bxr_json_reader_t *reader);

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
int bxr_json_reader_iter_object(bxr_json_reader_t *reader,
                                bxr_json_token_t *obj,
                                bxr_json_token_t *key,
                                bxr_json_token_t *value);

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
int bxr_json_reader_iter_array(bxr_json_reader_t *reader,
                               bxr_json_token_t *arr,
                               bxr_json_token_t *value);

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
bool bxr_json_reader_eq_str(bxr_json_token_t *key, const char *expected);

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
bool bxr_json_reader_eq_number(bxr_json_token_t *key, float expected);

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
bool bxr_json_reader_eq_bool(bxr_json_token_t *key, bool expected);

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
const char *bxr_json_read_token_str(bxr_json_token_t *token);

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
float bxr_json_read_token_number(bxr_json_token_t *token);

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
bool bxr_json_read_token_bool(bxr_json_token_t *token);

/**
 * ## Writer API
 */

typedef struct bxr_json_writer_s bxr_json_writer_t;

/**
 * Create a new JSON writer.
 *
 * `return` a pointer to the newly created JSON writer, or NULL if the writer
 * could not be created. Use `bxr_error_get` to get more information about the
 * error.
 *
 * The caller is responsible for destroying the returned JSON writer using
 * `bxr_json_destroy_writer` when it is no longer needed.
 */
bxr_json_writer_t *bxr_json_create_writer(void);

/**
 * Destroy a JSON writer and free its resources.
 *
 * `json` is the JSON writer to destroy.
 */
void bxr_json_destroy_writer(bxr_json_writer_t *json);

/**
 * Begin a new JSON object in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the object was successfully begun, or false if an error
 * occurred.
 */
bool bxr_json_writer_object_begin(bxr_json_writer_t *json);

/**
 * End the current JSON object in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the object was successfully ended, or false if an error
 * occurred.
 */
bool bxr_json_writer_object_end(bxr_json_writer_t *json);

/**
 * Begin a new JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the array was successfully begun, or false if an error
 * occurred.
 */
bool bxr_json_writer_array_begin(bxr_json_writer_t *json);

/**
 * End the current JSON array in the writer.
 *
 * `json` is the JSON writer to use.
 *
 * `return` true if the array was successfully ended, or false if an error
 * occurred.
 */
bool bxr_json_writer_array_end(bxr_json_writer_t *json);

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
bool bxr_json_write_key(bxr_json_writer_t *json, const char *key);

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
bool bxr_json_write_str(bxr_json_writer_t *json, const char *value);

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
bool bxr_json_write_number(bxr_json_writer_t *json, float number);

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
bool bxr_json_write_bool(bxr_json_writer_t *json, bool value);

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
bool bxr_json_write_key_str(bxr_json_writer_t *json,
                            const char *key,
                            const char *value);

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
bool bxr_json_write_key_number(bxr_json_writer_t *json,
                               const char *key,
                               float number);

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
bool
bxr_json_write_key_bool(bxr_json_writer_t *json, const char *key, bool value);

/**
 * Get the JSON data written by the writer.
 *
 * `json` is the JSON writer to get the data from.
 *
 * `length` is an optional output parameter to store the length of the returned
 * data in bytes.
 *
 * `return` a pointer to the JSON data written by the writer. The caller should
 * not modify or free this pointer, as it is owned by the JSON writer.
 */
const Uint8 *bxr_json_writer_get_data(bxr_json_writer_t *json, size_t *length);

#endif // BXR_JSON_H_
