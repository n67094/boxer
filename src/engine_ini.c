#include <SDL3/SDL.h>

#include <physfs.h>
#include <physfssdl3.h>

#include "engine_ini.h"
#include "engine_io.h"
#include "engine_mem.h"
#include "engine_str.h"

struct engine_inir_s
{
  char *data;
  char *end;
};

static char *
engine_inir_discard_line(engine_inir_t *ini, char *cursor)
{
  while (cursor < ini->end && *cursor != '\n') {
    *cursor = '\0'; // Null-terminate for simplify parsing
    cursor++;
  }
  return cursor;
}

// Unescape character in quoted value and move cursor to the end of the value.
static char *
engine_inir_unescape_quoted_value(engine_inir_t *ini, char *cursor)
{
  char *dest = cursor;
  cursor++; // Skip opening quote

  while (cursor < ini->end && *cursor != '"' && *cursor != '\n'
         && *cursor != '\r') {
    if (*cursor == '\\') {
      // Esacped char

      cursor++;
      switch (*cursor) {
      default:
        *dest = *cursor;
        break;
      case 'r':
        *dest = '\r';
        break;
      case 'n':
        *dest = '\n';
        break;
      case 't':
        *dest = '\t';
        break;
      case '\r':
      case '\n':
      case '\0':
        goto end; // End of line or buffer, stop parsing
      }

    } else {
      // Normal char
      *dest = *cursor;
    }
    cursor++;
    dest++;
  }

end:
  *dest = '\0';

  // Skip closing quote if present
  if (cursor < ini->end && *cursor == '"') {
    *cursor = '\0'; // Null-terminate for simplify parsing
    cursor++;
  }

  return cursor;
}

// Move cursor to the start of next line and return it.
static char *
engine_inir_next(engine_inir_t *ini, char *cursor)
{
  cursor += SDL_strlen(cursor); // Skip remaining non-empty characters
  while (cursor < ini->end && *cursor == '\0') {
    cursor++;
  }

  return cursor;
}

static void
engine_inir_trim_back(char *cursor, char *start)
{
  while (cursor > start
         && (*cursor == ' ' || *cursor == '\t' || *cursor == '\r')) {
    *cursor = '\0';
    cursor--;
  }
}

static void
engine_inir_parse(engine_inir_t *ini)
{
  char *cursor = ini->data;

  while (cursor < ini->end) {
    switch (*cursor) {
    case '\r': // Ignore carriage return for Windows-style line endings
    case '\n':
    case '\t':
    case ' ':
      *cursor = '\0'; // Null-terminate for simplify parsing
      // Fall through
    case '\0':
      cursor++;
      break;
    case '[':
      cursor += engine_strcspn(cursor, "]\n"); // Skip to ']' or end of line
      if (*cursor == '\n') {
        cursor = engine_inir_discard_line(ini, cursor);
        break;
      }
      *cursor = '\0'; // Null-terminate section name
      break;
    case ';':
      cursor = engine_inir_discard_line(ini, cursor);
      break;
    default: {
      char *line_start = cursor;

      cursor += engine_strcspn(cursor, "=\n");

      // If line is missing a '=', ignore the line
      if (*cursor != '=') {
        cursor = engine_inir_discard_line(ini, line_start);
        break;
      }

      // Trim whitespace before '=' for key
      engine_inir_trim_back(cursor - 1, line_start);

      // Trim '=' and whitespace after it
      do {
        *cursor++ = '\0';
      } while (*cursor == ' ' || *cursor == '\r' || *cursor == '\t');

      if (*cursor == '\n' || *cursor == '\0' || *cursor == ';') {
        cursor = engine_inir_discard_line(ini, line_start);
        break;
      }

      // Handle value
      if (*cursor == '"') {
        // Quoted string vlaue
        char *value_start = cursor;
        cursor            = engine_inir_unescape_quoted_value(ini, cursor);

        // If the string is empty, ignore the line
        if (cursor == value_start) {
          cursor = engine_inir_discard_line(ini, line_start);
          break;
        }

        // Trim whitespace after closing quote
        cursor = engine_inir_discard_line(ini, cursor);

      } else {
        // Normal value
        cursor += engine_strcspn(cursor, "\n");

        // Trim whitespace at the end of value
        engine_inir_trim_back(cursor - 1, line_start);
      }
      break;
    }
    }
  }
}

engine_inir_t *
engine_inir(const char *path)
{
  SDL_assert(path);

  engine_inir_t *ini = NULL;
  ENGINE_NEW(ini);
  if (!ini) {
    return NULL;
  }

  size_t length = 0;
  ini->data     = engine_io_read(path, &length);

  if (!ini->data) {
    ENGINE_FREE(ini);
    return NULL;
  }

  ini->end = ini->data + length;

  engine_inir_parse(ini);

  return ini;
}

const char *
engine_inir_str(engine_inir_t *ini, const char *section, const char *key)
{
  SDL_assert(ini);
  SDL_assert(section);
  SDL_assert(key);

  char *current_section = "";
  char *value           = NULL;
  char *cursor          = ini->data;

  size_t section_len = SDL_strlen(section);
  size_t key_len     = SDL_strlen(key);

  // Skip empty characters at the beginning
  if (*cursor == '\0') {
    cursor = engine_inir_next(ini, cursor);
  }

  while (cursor < ini->end) {
    if (*cursor == '[') {
      // Handle section
      current_section = cursor + 1; // Skip opening bracket
    } else {
      // Handle key
      value = engine_inir_next(ini,
                               cursor); // Move cursor to the start of the value

      if (SDL_strlen(current_section) == section_len
          && !SDL_strncasecmp(section, current_section, section_len)) {
        if (SDL_strlen(cursor) == key_len
            && !SDL_strncasecmp(key, cursor, key_len)) {
          return value;
        }
      }
      cursor = value;
    }

    cursor = engine_inir_next(ini, cursor);
  }

  return NULL;
}

void
engine_inir_destroy(engine_inir_t *ini)
{
  if (ini) {
    ENGINE_FREE(ini->data);
    ENGINE_FREE(ini);
  }
}

float
engine_inir_number(engine_inir_t *ini, const char *section, const char *key)
{
  SDL_assert(ini);
  SDL_assert(section);
  SDL_assert(key);

  const char *str_value = engine_inir_str(ini, section, key);

  if (!str_value) {
    return 0.0f; // Default value if not found
  }

  return (float)SDL_strtod(str_value, NULL);
}

const char *
engine_inir_str_or_else(engine_inir_t *ini,
                        const char *section,
                        const char *key,
                        const char *default_value)
{
  const char *value = engine_inir_str(ini, section, key);

  return value ? value : default_value;
}

float
engine_inir_number_or_else(engine_inir_t *ini,
                           const char *section,
                           const char *key,
                           float default_value)
{
  const char *str_value = engine_inir_str(ini, section, key);

  if (!str_value) {
    return default_value; // Default value if not found
  }

  return (float)SDL_strtod(str_value, NULL);
}

// ----------------------------------------------------------------------------

struct engine_iniw_s
{
  bool in_section;
  char *data;
  size_t size;
  size_t capacity;
};

static bool
engine_iniw_append(engine_iniw_t *ini, const char *str)
{
  size_t len      = str ? SDL_strlen(str) : 0;
  size_t new_size = ini->size + len;

  while (new_size >= ini->capacity) {
    size_t new_capacity = ini->capacity * 2;
    char *new_data      = NULL;
    ENGINE_ALLOC(new_data, new_capacity);
    if (new_data == NULL) {
      return false;
    }
    SDL_memcpy(new_data, ini->data, ini->size);
    ENGINE_FREE(ini->data);

    ini->data     = new_data;
    ini->capacity = new_capacity;
  }

  SDL_strlcat(ini->data, str, ini->capacity);
  ini->size += len;

  return true;
}

static bool
engine_iniw_append_escaped(engine_iniw_t *ini, const char *str)
{
  if (!engine_iniw_append(ini, "\"")) {
    return false;
  }

  bool is_ok = true;

  for (const char *c = str; *c && is_ok; c++) {
    switch (*c) {
    case '\"':
      is_ok = engine_iniw_append(ini, "\\\"");
      break;
    case '\\':
      is_ok = engine_iniw_append(ini, "\\\\");
      break;
    case '\b':
      is_ok = engine_iniw_append(ini, "\\b");
      break;
    case '\f':
      is_ok = engine_iniw_append(ini, "\\f");
      break;
    case '\n':
      is_ok = engine_iniw_append(ini, "\\n");
      break;
    case '\r':
      is_ok = engine_iniw_append(ini, "\\r");
      break;
    case '\t':
      is_ok = engine_iniw_append(ini, "\\t");
      break;
    default:
      if ((unsigned char)*c < 0x20) {
        char buf[7];
        SDL_snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)*c);
        is_ok = engine_iniw_append(ini, buf);
      } else {
        char buf[2] = { *c, '\0' };
        is_ok       = engine_iniw_append(ini, buf);
      }
      break;
    }
  }

  if (is_ok) {
    is_ok = engine_iniw_append(ini, "\"");
  }

  return is_ok;
}

engine_iniw_t *
engine_iniw(void)
{
  engine_iniw_t *ini = NULL;
  ENGINE_NEW(ini);

  ini->data = NULL;
  ENGINE_ALLOC(ini->data, ENGINE_INI_DEFAULT_DATA_CAPACITY);
  if (ini->data == NULL) {
    ENGINE_FREE(ini);
    return NULL;
  }
  ini->data[0]  = '\0';
  ini->size     = 0;
  ini->capacity = ENGINE_INI_DEFAULT_DATA_CAPACITY;

  ini->in_section = false;

  return ini;
}

void
engine_iniw_destroy(engine_iniw_t *ini)
{
  if (ini) {
    ENGINE_FREE(ini->data);
    ENGINE_FREE(ini);
  }
}

bool
engine_iniw_section_begin(engine_iniw_t *ini, const char *section)
{
  SDL_assert(ini);
  SDL_assert(section);
  SDL_assert(!ini->in_section);

  size_t line_len = SDL_strlen(section) + 4; // + 3 for [ ] \n and \0
  char *line      = NULL;
  ENGINE_ALLOC(line, line_len);
  if (line == NULL) {
    return false;
  }

  SDL_snprintf(line, line_len, "[%s]\n", section);

  if (!engine_iniw_append(ini, line)) {
    ENGINE_FREE(line);
    return false;
  }

  ENGINE_FREE(line);
  ini->in_section = true;
  return true;
}

bool
engine_iniw_key_str(engine_iniw_t *ini, const char *key, const char *value)
{
  SDL_assert(ini);
  SDL_assert(key);
  SDL_assert(value);
  SDL_assert(ini->in_section);

  size_t left_len = SDL_strlen(key) + 2; // + 1 for = and + 1 for \n
  char *left      = NULL;
  ENGINE_ALLOC(left, left_len + 2);
  if (left == NULL) {
    return false;
  }

  SDL_snprintf(left, left_len, "%s=", key);

  if (!engine_iniw_append(ini, left)) {
    ENGINE_FREE(left);
    return false;
  }

  ENGINE_FREE(left);

  if (!engine_iniw_append_escaped(ini, value)) {
    return false;
  }

  engine_iniw_append(ini, "\n");

  return true;
}

bool
engine_iniw_key_number(engine_iniw_t *ini, const char *key, float number)
{
  SDL_assert(ini);
  SDL_assert(key);
  SDL_assert(ini->in_section);

  char number_str[32];
  SDL_snprintf(number_str, sizeof(number_str), "%g", number);

  size_t line_len
      = SDL_strlen(key) + SDL_strlen(number_str) + 3; // + 2 for = \n and \0
  char *line = NULL;
  ENGINE_ALLOC(line, line_len);
  if (line == NULL) {
    return false;
  }

  SDL_snprintf(line, line_len, "%s=%s\n", key, number_str);

  if (!engine_iniw_append(ini, line)) {
    ENGINE_FREE(line);
    return false;
  }

  ENGINE_FREE(line);
  return true;
}

bool
engine_iniw_section_end(engine_iniw_t *ini)
{
  SDL_assert(ini);
  SDL_assert(ini->in_section);

  if (!engine_iniw_append(ini, "\n")) {
    return false;
  }

  ini->in_section = false;

  return true;
}

bool
engine_iniw_save(engine_iniw_t *ini, const char *path)
{
  SDL_assert(ini);
  SDL_assert(path);

  size_t data_len = SDL_strlen(ini->data);

  return engine_io_write(path, ini->data, data_len, false);
}
