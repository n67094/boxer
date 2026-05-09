<a name="line-1"></a>
File `bxr_json.h`.

Copyright (c) 2026 nsix. All rights reserved.

# JSON File Reader and Writer

Support objects, arrays, strings, numbers and booleans.

File extension doesn't matter, but recommended to use `.json`.

<a name="line-45"></a>
## Reader API

<a name="line-49"></a>

```cpp
bxr_json_reader_t *bxr_json_create_reader(const char *path)
```

Load a JSON file from the given path and return a reader for it.

`path` is the path to the JSON file to load. (c.f `bxr_io_read` for path
resolution).

`return` a reader for the loaded JSON file, or NULL if an error occurred. Use
`bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned reader using
`bxr_json_destroy_reader` when it is no longer needed.

---

<a name="line-63"></a>

```cpp
void bxr_json_destroy_reader(bxr_json_reader_t *reader)
```

Destroy a JSON reader and free its resources.

`reader` is the JSON reader to destroy.

---

<a name="line-70"></a>

```cpp
bxr_json_token_t bxr_json_read_token(bxr_json_reader_t *reader)
```

Read the next token from the JSON reader.

`reader` is the JSON reader to read from.

`return` the next token read from the JSON reader, or a token with type
`BXR_JSON_ERROR` if an error occurred while reading. Use `bxr_error_get` to
get more information about the error.

The caller should not modify or free these pointers, as they are owned by the
JSON reader.

---

<a name="line-84"></a>

```cpp
int bxr_json_reader_iter_object(bxr_json_reader_t *reader,
```

Iterate over the key-value pairs in a JSON object.

`reader` is the JSON reader to read from.

`obj` is the token representing the JSON object to iterate over. This token
should have been returned by a previous call to `bxr_jsonr_read`.

`key` is an output parameter to store the token representing the current key
in the iteration.

`value` is an output parameter to store the token representing the current
value in the iteration.

`return` 1 if a key-value pair was successfully read, 0 if the end of the
object was reached, or -1 if an error occurred while reading.

---

<a name="line-106"></a>

```cpp
int bxr_json_reader_iter_array(bxr_json_reader_t *reader,
```

Iterate over the values in a JSON array.

`reader` is the JSON reader to read from.

`arr` is the token representing the JSON array to iterate over. This token
should have been returned by a previous call to `bxr_jsonr_read`.

`value` is an output parameter to store the token representing the current
value in the iteration.

`return` 1 if a value was successfully read, 0 if the end of the array was
reached, or -1 if an error occurred while reading.

---

<a name="line-124"></a>

```cpp
bool bxr_json_reader_eq_str(bxr_json_token_t *key, const char *expected)
```

Compare a JSON token with an expected string value.

`key` is the JSON token to compare. This token should have been returned by a
previous call to `bxr_jsonr_read` and should have type `BXR_JSON_STRING`.

`expected` is the expected string value to compare against.

`return` true if the JSON token's string value matches the expected string,
or false otherwise.

---

<a name="line-137"></a>

```cpp
bool bxr_json_reader_eq_number(bxr_json_token_t *key, float expected)
```

Compare a JSON token with an expected number value.

`key` is the JSON token to compare. This token should have been returned by a
previous call to `bxr_jsonr_read` and should have type `BXR_JSON_NUMBER`.

`expected` is the expected number value to compare against.

`return` true if the JSON token's number value matches the expected number,
or false otherwise.

---

<a name="line-150"></a>

```cpp
bool bxr_json_reader_eq_bool(bxr_json_token_t *key, bool expected)
```

Compare a JSON token with an expected boolean value.

`key` is the JSON token to compare. This token should have been returned by a
previous call to `bxr_jsonr_read` and should have type `BXR_JSON_BOOL`.

`expected` is the expected boolean value to compare against.

`return` true if the JSON token's boolean value matches the expected boolean,
or false otherwise.

---

<a name="line-163"></a>

```cpp
const char *bxr_json_read_token_str(bxr_json_token_t *token)
```

Get the string value of a JSON token.

`token` is the JSON token to get the string value from. This token should
have been returned by a previous call to `bxr_jsonr_read` and should have
type `BXR_JSON_STRING`.

`return` a pointer to the string value of the JSON token, or NULL if the
token does not have a string value.

---

<a name="line-175"></a>

```cpp
float bxr_json_read_token_number(bxr_json_token_t *token)
```

Get the number value of a JSON token.

`token` is the JSON token to get the number value from. This token should
have been returned by a previous call to `bxr_jsonr_read` and should have
type `BXR_JSON_NUMBER`.

`return` the number value of the JSON token, or 0.0 if the token does not
have a number value.

---

<a name="line-187"></a>

```cpp
bool bxr_json_read_token_bool(bxr_json_token_t *token)
```

Get the boolean value of a JSON token.

`token` is the JSON token to get the boolean value from. This token should
have been returned by a previous call to `bxr_jsonr_read` and should have
type `BXR_JSON_BOOL`.

`return` the boolean value of the JSON token, or false if the token does not
have a boolean value.

---

<a name="line-199"></a>
## Writer API

<a name="line-205"></a>

```cpp
bxr_json_writer_t *bxr_json_create_writer(void)
```

Create a new JSON writer.

`return` a pointer to the newly created JSON writer, or NULL if the writer
could not be created. Use `bxr_error_get` to get more information about the
error.

The caller is responsible for destroying the returned JSON writer using
`bxr_json_destroy_writer` when it is no longer needed.

---

<a name="line-217"></a>

```cpp
void bxr_json_destroy_writer(bxr_json_writer_t *json)
```

Destroy a JSON writer and free its resources.

`json` is the JSON writer to destroy.

---

<a name="line-224"></a>

```cpp
bool bxr_json_writer_object_begin(bxr_json_writer_t *json)
```

Begin a new JSON object in the writer.

`json` is the JSON writer to use.

`return` true if the object was successfully begun, or false if an error
occurred.

---

<a name="line-234"></a>

```cpp
bool bxr_json_writer_object_end(bxr_json_writer_t *json)
```

End the current JSON object in the writer.

`json` is the JSON writer to use.

`return` true if the object was successfully ended, or false if an error
occurred.

---

<a name="line-244"></a>

```cpp
bool bxr_json_writer_array_begin(bxr_json_writer_t *json)
```

Begin a new JSON array in the writer.

`json` is the JSON writer to use.

`return` true if the array was successfully begun, or false if an error
occurred.

---

<a name="line-254"></a>

```cpp
bool bxr_json_writer_array_end(bxr_json_writer_t *json)
```

End the current JSON array in the writer.

`json` is the JSON writer to use.

`return` true if the array was successfully ended, or false if an error
occurred.

---

<a name="line-264"></a>

```cpp
bool bxr_json_write_key(bxr_json_writer_t *json, const char *key)
```

Add a key-value pair with a string value to the current JSON object in the
writer.

`json` is the JSON writer to use.

`key` is the name of the key to add.

`value` is the string value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred.

---

<a name="line-279"></a>

```cpp
bool bxr_json_write_str(bxr_json_writer_t *json, const char *value)
```

Add a string value to the current JSON array in the writer.

`json` is the JSON writer to use.

`value` is the string value to add to the array.

`return` true if the value was successfully added, or false if an error
occurred.

---

<a name="line-291"></a>

```cpp
bool bxr_json_write_number(bxr_json_writer_t *json, float number)
```

Add a number value to the current JSON array in the writer.

`json` is the JSON writer to use.

`number` is the number value to add to the array.

`return` true if the value was successfully added, or false if an error
occurred.

---

<a name="line-303"></a>

```cpp
bool bxr_json_write_bool(bxr_json_writer_t *json, bool value)
```

Add a boolean value to the current JSON array in the writer.

`json` is the JSON writer to use.

`value` is the boolean value to add to the array.

`return` true if the value was successfully added, or false if an error
occurred.

---

<a name="line-315"></a>

```cpp
bool bxr_json_write_key_str(bxr_json_writer_t *json,
```

Add a key-value pair with a string value to the current JSON object in the
writer.

`json` is the JSON writer to use.

`key` is the name of the key to add.

`value` is the string value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred.

---

<a name="line-332"></a>

```cpp
bool bxr_json_write_key_number(bxr_json_writer_t *json,
```

Add a key-value pair with a number value to the current JSON object in the
writer.

`json` is the JSON writer to use.

`key` is the name of the key to add.

`number` is the number value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred.

---

<a name="line-349"></a>

```cpp
bool
```

Add a key-value pair with a boolean value to the current JSON object in the
writer.

`json` is the JSON writer to use.

`key` is the name of the key to add.

`value` is the boolean value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred.

---

<a name="line-365"></a>

```cpp
bool bxr_json_writer_save(bxr_json_writer_t *json, const char *path)
```

Save the JSON data from the writer to a file at the given path.

`json` is the JSON writer containing the data to save.

`path` is the path to the file where the JSON data should be saved. (c.f
`bxr_io_write` for path resolution).

`return` true if the JSON data was successfully saved, or false if an error
occurred.

---
