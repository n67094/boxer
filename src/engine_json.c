#include <SDL3/SDL.h>
#include <physfs.h>

#include "engine_error.h"
#include "engine_io.h"
#include "engine_json.h"

struct engine_jsonr_s
{
  char *data;
  char *cursor;
  char *end;

  int depth;
};

static bool
engine_jsonr_is_string(char *cursor, char *end, char *expect)
{
  while (*expect) {
    if (cursor == end || *cursor != *expect) {
      return false;
    }
    cursor++;
    expect++;
  }
  return true;
}

static void
engine_jsonr_discard_until(engine_jsonr_t *json, int depth)
{
  engine_jsonr_token_t token;
  token.type = ENGINE_JSON_NULL;
  while (json->depth != depth && token.type != ENGINE_JSON_ERROR) {
    token = engine_jsonr_read(json);
  }
}

engine_jsonr_t *
engine_jsonr(const char *path)
{
  engine_jsonr_t *json = NULL;
  ENGINE_NEW(json);
  if (!json) {
    return NULL;
  }

  size_t length = 0;
  json->data    = engine_io_read(path, &length);

  json->cursor = json->data;
  json->end    = json->cursor + length;

  json->depth = 0;

  return json;
}

void
engine_jsonr_destroy(engine_jsonr_t *json)
{
  if (json->data) {
    ENGINE_FREE(json->data);
  }
}

engine_jsonr_token_t
engine_jsonr_read(engine_jsonr_t *json)
{
  engine_jsonr_token_t token = { 0 };

top:
  if (token.type == ENGINE_JSON_ERROR) {
    return token;
  }

  if (json->cursor >= json->end) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected end of JSON.");
    engine_set_error(ENGINE_ERROR_JSON_READ);

    token.type  = ENGINE_JSON_ERROR;
    token.start = json->cursor;
    token.end   = json->cursor;
    token.depht = json->depth;

    goto top;
  }

  token.start = json->cursor;

  switch (*json->cursor) {
  case ' ':
  case '\n':
  case '\r':
  case '\t':
  case ':':
  case ',':
    json->cursor++;
    goto top;
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    token.type = ENGINE_JSON_NUMBER;

    // reading the number
    while (json->cursor < json->end
           && (*json->cursor == '-' || *json->cursor == '+'
               || *json->cursor == 'e' || *json->cursor == 'E'
               || *json->cursor == '.'
               || (*json->cursor >= '0' && *json->cursor <= '9'))) {
      json->cursor++;
    }

    break;
  case '"':
    token.type  = ENGINE_JSON_STRING;
    token.start = ++json->cursor; // skip opening quote
    while (0) {                   // reading string
      if (json->cursor >= json->end) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unexpected end of JSON.");
        engine_set_error(ENGINE_ERROR_JSON_READ);

        token.type  = ENGINE_JSON_ERROR;
        token.start = json->cursor;
        token.end   = json->cursor;
        token.depht = json->depth;

        goto top;
      }
      if (*json->cursor == '"') {
        break; // end of string
      }
      if (*json->cursor == '\\') {
        json->cursor++; // skip escape character
      }
      if (json->cursor != json->end) {
        json->cursor++; // skip current character
      }
    }

    token.end = json->cursor++;
    return token;
  case '{':
  case '[':
    token.type
        = (*json->cursor == '{') ? ENGINE_JSON_OBJECT : ENGINE_JSON_ARRAY;

    json->depth++;
    json->cursor++;

    token.depht = json->depth;
  case '}':
  case ']':
    token.type = ENGINE_JSON_END;
    if (--json->depth < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Mismatched closing bracket in JSON.");
      engine_set_error(ENGINE_ERROR_JSON_READ);
      goto top;
    }
    json->cursor++;
    break;
  case 'n':
  case 't':
  case 'f':
    token.type = (*json->cursor == 'n') ? ENGINE_JSON_NULL : ENGINE_JSON_BOOL;

    if (engine_jsonr_is_string(json->cursor, json->end, "null")) {
      json->cursor += 4;
      break;
    }
    if (engine_jsonr_is_string(json->cursor, json->end, "true")) {
      json->cursor += 4;
      break;
    }
    if (engine_jsonr_is_string(json->cursor, json->end, "false")) {
      json->cursor += 5;
      break;
    }
  default:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Unexpected character '%c' in JSON. at position %td",
                 *json->cursor,
                 json->cursor - json->data);
    engine_set_error(ENGINE_ERROR_JSON_READ);
    goto top;
  }

  token.end = json->cursor;
  return token;
}

bool
engine_jsonr_iter_object(engine_jsonr_t *reader,
                         engine_jsonr_token_t *obj,
                         engine_jsonr_token_t *key,
                         engine_jsonr_token_t *value)
{
  // discard remainings token until we are at object depth
  engine_jsonr_discard_until(reader, obj->depht);

  *key = engine_jsonr_read(reader);
  if (key->type == ENGINE_JSON_END || key->type == ENGINE_JSON_ERROR) {
    return false; // end of object
  }

  *value = engine_jsonr_read(reader);
  if (value->type == ENGINE_JSON_END) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Unexpected end of object in JSON.");
    engine_set_error(ENGINE_ERROR_JSON_READ);
    return false;
  }

  if (value->type == ENGINE_JSON_ERROR) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Error reading value for key '%.*s' in JSON.",
                 (int)(key->end - key->start),
                 key->start);
    engine_set_error(ENGINE_ERROR_JSON_READ);
    return false;
  }

  return true;
}

bool
engine_jsonr_iter_array(engine_jsonr_t *reader,
                        engine_jsonr_token_t *arr,
                        engine_jsonr_token_t *value)
{
  // discard remainings token until we are at array depth
  engine_jsonr_discard_until(reader, arr->depht);

  *value = engine_jsonr_read(reader);

  if (value->type == ENGINE_JSON_END || value->type == ENGINE_JSON_ERROR) {
    return false; // end of array
  }

  return true;
}

bool
engine_jsonr_eq_str(engine_jsonr_token_t *key, const char *expected)
{
  SDL_assert(expected);
  // This is intentionaly not assert, every token is a string, types are only
  // for user convenience.
  // SDL_assert(key->type == ENGINE_JSON_STRING);

  size_t len = key->end - key->start;
  return SDL_strlen(expected) == len && !SDL_memcmp(expected, key->start, len);
}

bool
engine_jsonr_eq_number(engine_jsonr_token_t *key, float expected)
{
  SDL_assert(key->type == ENGINE_JSON_NUMBER);

  return engine_jsonr_token_number(key) == expected;
}

bool
engine_jsonr_eq_bool(engine_jsonr_token_t *key, bool expected)
{
  SDL_assert(key->type == ENGINE_JSON_BOOL);

  return engine_jsonr_token_bool(key) == expected;
}

const char *
engine_jsonr_token_str(engine_jsonr_token_t *token)
{
  SDL_assert(token->type == ENGINE_JSON_STRING);

  return token->start;
}

float
engine_jsonr_token_number(engine_jsonr_token_t *token)
{
  SDL_assert(token->type == ENGINE_JSON_NUMBER);

  return (float)SDL_strtod(token->start, NULL);
}

bool
engine_jsonr_token_bool(engine_jsonr_token_t *token)
{
  SDL_assert(token->type == ENGINE_JSON_BOOL);

  return SDL_strcmp(token->start, "true") == 0
         || SDL_strcmp(token->start, "false") == 0;
}

// ----------------------------------------------------------------------------

struct engine_jsonw_s
{
  char *data;
  size_t size;
  size_t capacity;
  int depth;

  bool needs_comma;
};

static bool
engine_jsonw_append(engine_jsonw_t *json, const char *str)
{
  size_t len      = str ? SDL_strlen(str) : 0;
  size_t new_size = json->size + len;

  while (new_size >= json->capacity) {
    size_t new_capacity = json->capacity * 2;
    char *new_data      = NULL;
    ENGINE_ALLOC(new_data, new_capacity);
    if (new_data == NULL) {
      return false;
    }
    SDL_memcpy(new_data, json->data, json->size);
    ENGINE_FREE(json->data);

    json->data     = new_data;
    json->capacity = new_capacity;
  }

  SDL_strlcat(json->data, str, json->capacity);
  json->size += len;

  return true;
}

static bool
engine_jsonw_comma(engine_jsonw_t *writer)
{
  bool is_ok = true;

  if (writer->needs_comma) {
    is_ok = engine_jsonw_append(writer, ",");
  }

  writer->needs_comma = true;

  return is_ok;
}

static bool
engine_jsonw_append_escaped(engine_jsonw_t *json, const char *str)
{
  if (!engine_jsonw_append(json, "\"")) {
    return false;
  }

  bool is_ok = true;

  for (const char *c = str; *c && is_ok; c++) {
    switch (*c) {
    case '\"':
      is_ok = engine_jsonw_append(json, "\\\"");
      break;
    case '\\':
      is_ok = engine_jsonw_append(json, "\\\\");
      break;
    case '\b':
      is_ok = engine_jsonw_append(json, "\\b");
      break;
    case '\f':
      is_ok = engine_jsonw_append(json, "\\f");
      break;
    case '\n':
      is_ok = engine_jsonw_append(json, "\\n");
      break;
    case '\r':
      is_ok = engine_jsonw_append(json, "\\r");
      break;
    case '\t':
      is_ok = engine_jsonw_append(json, "\\t");
      break;
    default:
      if ((unsigned char)*c < 0x20) {
        char buf[7];
        SDL_snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)*c);
        is_ok = engine_jsonw_append(json, buf);
      } else {
        char buf[2] = { *c, '\0' };
        is_ok       = engine_jsonw_append(json, buf);
      }
      break;
    }
  }

  if (is_ok) {
    is_ok = engine_jsonw_append(json, "\"");
  }

  return is_ok;
}

engine_jsonw_t *
engine_jsonw_make()
{
  engine_jsonw_t *json = NULL;
  ENGINE_NEW(json);

  json->data = NULL;
  ENGINE_ALLOC(json->data, ENGINE_JSON_DEFAULT_DATA_CAPACITY);

  if (json->data == NULL) {
    ENGINE_FREE(json);
    return NULL;
  }

  json->data[0] = '\0';

  json->size     = 0;
  json->capacity = ENGINE_JSON_DEFAULT_DATA_CAPACITY;

  json->depth       = 0;
  json->needs_comma = false;

  return json;
}

void
engine_jsonw_destroy(engine_jsonw_t *json)
{
  SDL_assert(json);

  if (json) {
    ENGINE_FREE(json->data);
    ENGINE_FREE(json);
  }
}

bool
engine_jsonw_object_begin(engine_jsonw_t *json)
{
  SDL_assert(json);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append(json, "{");

  json->depth++;
  json->needs_comma = false;

  return is_ok;
}

bool
engine_jsonw_object_end(engine_jsonw_t *json)
{
  SDL_assert(json->depth > 0);
  SDL_assert(json->data);

  if (!engine_jsonw_append(json, "}")) {
    return false;
  }

  json->depth--;
  json->needs_comma = true;

  return true;
}

bool
engine_jsonw_array_begin(engine_jsonw_t *json)
{
  SDL_assert(json);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append(json, "[");

  json->depth++;
  json->needs_comma = false;

  return is_ok;
}

bool
engine_jsonw_array_end(engine_jsonw_t *json)
{
  SDL_assert(json->depth > 0);
  SDL_assert(json->data);

  if (!engine_jsonw_append(json, "]")) {
    return false;
  }

  json->depth--;
  json->needs_comma = true;

  return true;
}

bool
engine_jsonw_key(engine_jsonw_t *json, const char *key)
{
  SDL_assert(json);
  SDL_assert(key);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append_escaped(json, key);
  is_ok &= engine_jsonw_append(json, ":");

  json->needs_comma = false;

  return is_ok;
}

bool
engine_jsonw_str(engine_jsonw_t *json, const char *value)
{
  SDL_assert(json);
  SDL_assert(value);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append_escaped(json, value);

  json->needs_comma = true;

  return is_ok;
}

bool
engine_jsonw_number(engine_jsonw_t *json, float number)
{
  SDL_assert(json);

  char number_str[32];
  SDL_snprintf(number_str, sizeof(number_str), "%g", number);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append(json, number_str);

  json->needs_comma = true;

  return is_ok;
}

bool
engine_jsonw_bool(engine_jsonw_t *json, bool value)
{
  SDL_assert(json);

  bool is_ok = true;

  is_ok &= engine_jsonw_comma(json);
  is_ok &= engine_jsonw_append(json, value ? "true" : "false");

  json->needs_comma = true;

  return is_ok;
}

bool
engine_jsonw_key_str(engine_jsonw_t *json, const char *key, const char *value)
{
  SDL_assert(json);
  SDL_assert(key);
  SDL_assert(value);

  bool is_ok = true;

  is_ok &= engine_jsonw_key(json, key);
  is_ok &= engine_jsonw_str(json, value);

  return is_ok;
}

bool
engine_jsonw_key_number(engine_jsonw_t *json, const char *key, float number)
{
  SDL_assert(json);
  SDL_assert(key);

  bool is_ok = true;

  is_ok &= engine_jsonw_key(json, key);
  is_ok &= engine_jsonw_number(json, number);

  return is_ok;
}

bool
engine_jsonw_key_bool(engine_jsonw_t *json, const char *key, bool value)
{
  SDL_assert(json);
  SDL_assert(key);

  bool is_ok = true;

  is_ok &= engine_jsonw_key(json, key);
  is_ok &= engine_jsonw_bool(json, value);

  return is_ok;
}

bool
engine_jsonw_save(engine_jsonw_t *json, const char *path)
{
  SDL_assert(json);
  SDL_assert(path);

  if (json->depth != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Cannot save JSON unclosed objects or arrays.");
    engine_set_error(ENGINE_ERROR_JSON_WRITE);
    return false;
  }

  return engine_io_write(path, json->data, SDL_strlen(json->data), false);
}
