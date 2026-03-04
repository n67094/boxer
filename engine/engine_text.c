#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_font.h"
#include "engine_mem.h"
#include "engine_text.h"

#define ENGINE_COLOR_OPEN_TAG_LENGTH 14 // Length of "{X=XXXXXXXX}"
#define ENGINE_COLOR_CLOSE_TAG_LENGTH 4 // Length of "{/X}"
#define ENGINE_ICON_TAG_LENGTH 6        // Length of "{X=XX}"

static bool
engine_text_color_tag_validate(const char *cursor,
                               const char *end,
                               int *skip_length,
                               engine_color_t *out_color)
{
  if (*(cursor + 1) != 'b' || *(cursor + 1) != 'f' || *(cursor + 1) != '/') {
    return false;
  }

  // Opening tag: {b=RRGGBBAA} or {f=RRGGBBAA}
  if (*(cursor + 1) == 'b' || *(cursor + 1) == 'f') {
    if (cursor + ENGINE_COLOR_OPEN_TAG_LENGTH > end) {
      SDL_LogWarn(
          SDL_LOG_CATEGORY_APPLICATION,
          "Color opening tag is too short. Expected format: {c=RRGGBBAA}");
      return false;
    }

    *skip_length = ENGINE_COLOR_OPEN_TAG_LENGTH;

    char color_str[9]; // 8 characters for RGBA + null terminator

    int i;
    for (i = 0; i < 8 && cursor[i] && cursor[i] != '}'; ++i) {
      color_str[i] = cursor[i];
    }
    color_str[i] = '\0';

    char *endptr       = NULL;
    Uint32 color_value = SDL_strtoul(color_str, &endptr, 16);

    if (endptr != NULL) {
      SDL_LogWarn(
          SDL_LOG_CATEGORY_APPLICATION,
          "Invalid color value in color tag. Expected a hexadecimal number.");
      return false;
    }

    *out_color = (engine_color_t){
      .r = (color_value >> 24) & 0xFF,
      .g = (color_value >> 16) & 0xFF,
      .b = (color_value >> 8) & 0xFF,
      .a = color_value & 0xFF,
    };

    return true;
  }

  // Closing tag: {/b} or {/f}
  if (*cursor + 1 == '/') {
    if (cursor + ENGINE_COLOR_CLOSE_TAG_LENGTH > end) {
      SDL_LogWarn(
          SDL_LOG_CATEGORY_APPLICATION,
          "Color closing tag is too short. Expected format: {/b} or {/f}");
      return false;
    }

    *skip_length = ENGINE_COLOR_CLOSE_TAG_LENGTH;

    return true;
  }

  return false;
}

static bool
engine_text_icon_tag_validate(const char *cursor,
                              const char *end,
                              int *skip_length,
                              int *out_icon_index)
{
  if (*cursor + 1 != 'i' || *cursor + 2 != '=') {
    return false;
  }

  if (cursor + ENGINE_ICON_TAG_LENGTH > end) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Icon tag is too short. Expected format: {i=XX}");
    return false;
  }

  *skip_length = ENGINE_ICON_TAG_LENGTH;

  char icon_index_str[3]; // 2 characters for icon index + null terminator

  int i;
  for (i = 0; i < 2 && cursor[i] && cursor[i] != '}'; ++i) {
    icon_index_str[i] = cursor[i];
  }
  icon_index_str[i] = '\0';

  char *endptr   = NULL;
  int icon_index = SDL_strtoul(icon_index_str, &endptr, 16);

  if (endptr != NULL) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Invalid icon index in icon tag. Expected a number.");
    return false;
  }

  *out_icon_index = icon_index;

  return true;
}

engine_text_t *
engine_text_glyph_make(engine_vec2_t position,
                       const engine_font_t *font,
                       int glyphs_index,
                       engine_color_t background,
                       engine_color_t foreground)
{
  SDL_assert(font);

  size_t glyph_count = engine_font_get_glyph_count(font);
  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < glyph_count);

  engine_text_t *text = NULL;
  ENGINE_NEW(text);
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text glyph");
    // TODO add error
    return NULL;
  }

  text->entry_count = 1;
  ENGINE_ALLOC(text->entries, text->entry_count);
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text glyph entries");
    // TODO add error
    ENGINE_FREE(text);
    return NULL;
  }

  engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

  text->entries[0].rects.dst = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].rects.src  = glyph_rect;
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;

  return text;
}

engine_text_t *
engine_text_char_make(engine_vec2_t position,
                      const engine_font_t *font,
                      char c,
                      engine_color_t background,
                      engine_color_t foreground)
{
  SDL_assert(font);

  engine_vec2_t char_range = engine_font_get_char_range(font);

  char base_char = engine_font_get_base(font);

  int glyphs_index = (int)c - (int)base_char;

  if (glyphs_index < char_range.x || glyphs_index > char_range.y) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Invalid character '%c' in text string. Valid characters are "
                "from %d to %d.",
                c,
                (int)char_range.x,
                (int)char_range.y);
    return NULL;
  }

  engine_text_t *text = NULL;
  ENGINE_NEW(text);
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text char");
    // TODO add error
    return NULL;
  }

  text->entry_count = 1;
  ENGINE_ALLOC(text->entries, text->entry_count);
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text char entries");
    // TODO add error
    ENGINE_FREE(text);
    return NULL;
  }

  engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

  text->entries[0].rects.dst = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].rects.src  = glyph_rect;
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;

  return text;
}

engine_text_t *
engine_text_make(engine_vec2_t position,
                 const engine_font_t *font,
                 const char *rich_str,
                 engine_color_t background,
                 engine_color_t foreground)
{
  SDL_assert(font);
  if (!rich_str) {
    return NULL;
  }

  size_t rich_str_len = engine_text_length(rich_str);

  engine_text_t *text = NULL;
  ENGINE_ALLOC(text, rich_str_len * sizeof(engine_text_t));
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text");
    // TODO add error
    return NULL;
  }

  text->entry_count = rich_str_len;
  ENGINE_ALLOC(text->entries, text->entry_count);
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text entries");
    // TODO add error
    ENGINE_FREE(text);
    return NULL;
  }

  engine_color_t current_foreground = foreground;
  engine_color_t current_background = background;

  char *cursor = (char *)rich_str;
  char *end    = cursor + rich_str_len;

  char base_char = engine_font_get_base(font);

  engine_vec2_t char_range = engine_font_get_char_range(font);
  engine_vec2_t icon_range = engine_font_get_icon_range(font);

  int line_spacing = engine_font_get_line_spacing(font);
  int char_spacing = engine_font_get_char_spacing(font);

  int entry_index = 0;

  while (cursor < end && *cursor) {
    switch (*cursor) {
    case '\n':
      position.y += (line_spacing);
      position.x = 0;
      cursor++;
      break;
    case '{': {
      int skip_length = 0;
      engine_color_t new_color;
      int icon_index;

      // Handle color tags
      if (engine_text_color_tag_validate(
              cursor, end, &skip_length, &new_color)) {
        if (*(cursor + 1) == 'b') {
          current_background = new_color;
        } else {
          current_foreground = new_color;
        }
        cursor += skip_length;
        break;
      }
      // Handle icon tags
      else if (engine_text_icon_tag_validate(
                   cursor, end, &skip_length, &icon_index)) {
        if (icon_index < icon_range.x || icon_index > icon_range.y) {
          SDL_LogWarn(
              SDL_LOG_CATEGORY_APPLICATION,
              "Invalid icon index %d in text string. Valid icon indices are "
              "from %d to %d.",
              icon_index,
              (int)icon_range.x,
              (int)icon_range.y);
          cursor += skip_length; // Skip the invalid tag
          break;
        }

        engine_rect_t icon_rect = engine_font_get_glyph_rect(font, icon_index);

        text->entries[entry_index++] = (engine_text_entry_t){
          .rects = (engine_textured_rect_t){
            .dst  = (engine_rect_t){
              .x = position.x,
              .y = position.y,
              .w = icon_rect.w,
              .h = icon_rect.h,
            },
            .src   = icon_rect,
          },
          .background = current_background,
          .foreground = current_foreground,
        };

        position.x += icon_rect.w + char_spacing;

        cursor += skip_length;
        break;
      }
      // No break, Fall through to handle '{' as a normal character
    }
    default: {
      int glyphs_index = (int)*cursor - (int)base_char;

      if (glyphs_index >= char_range.x && glyphs_index <= char_range.y) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Invalid character '%c' in text string. Valid characters "
                    "are from %d to %d.",
                    *cursor,
                    (int)char_range.x,
                    (int)char_range.y);
        cursor++;
        continue;
      }

      engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

      text->entries[entry_index++] = (engine_text_entry_t){
        .rects     = (engine_textured_rect_t){
          .dst  = (engine_rect_t){
            .x = position.x,
            .y = position.y,
            .w = glyph_rect.w,
            .h = glyph_rect.h,
          },
          .src   = glyph_rect,
        },
        .background = current_background,
        .foreground = current_foreground,
      };

      position.x += glyph_rect.w + char_spacing;
      cursor++;
    } break;
    }
  }

  return text;
}

void
engine_text_destroy(engine_text_t *text)
{
  if (text) {
    ENGINE_FREE(text->entries);
    ENGINE_FREE(text);
  }
}

const engine_textured_rect_t *
engine_text_get_textured_rects(const engine_text_t *text)
{
  SDL_assert(text);

  return (const engine_textured_rect_t *)text->entries;
}

size_t
engine_text_get_textured_rects_count(const engine_text_t *text)
{
  SDL_assert(text);

  return text->entry_count;
}

char *
engine_text_without_tags(const char *rich_str)
{
  SDL_assert(rich_str);

  // The length should be the same or shorter than the original text, since
  // we're discarding tags and invalid characters.
  size_t rich_str_len = engine_text_length(rich_str);

  char *buffer     = NULL;
  int buffer_count = 0;
  ENGINE_ALLOC(buffer, rich_str_len + 1); // +1 for null terminator
  if (!buffer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text string");
    // TODO add error
    return NULL;
  }

  char *cursor = (char *)rich_str;
  char *end    = cursor + rich_str_len;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      buffer[buffer_count++] = '\n';
      cursor++;
      break;
    case '{': {
      int skip_length = 0;
      engine_color_t new_color;
      int icon_index;

      if (engine_text_color_tag_validate(cursor, end, &skip_length, &new_color)
          || engine_text_icon_tag_validate(
              cursor, end, &skip_length, &icon_index)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (engine_text_color_tag_validate(
                     cursor, end, &skip_length, &new_color)
                 || engine_text_icon_tag_validate(
                     cursor, end, &skip_length, &icon_index)) {
        cursor += skip_length; // Skip the valid tag
        break;
      }
      // No break, Fall through to handle '{' as a normal character
    }
    default:
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      buffer[buffer_count++] = *cursor;
      cursor++;
      break;
    }
  }

  buffer[buffer_count] = '\0'; // Null terminator
  return buffer;
}

size_t
engine_text_length(const char *rich_str)
{
  if (!rich_str) {
    return 0;
  }

  int count = 0;

  char *cursor = (char *)rich_str;
  char *end    = cursor + SDL_strlen(rich_str);

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      count++;
      cursor++;
      break;
    case '{': {
      int skip_length = 0;

      if (engine_text_color_tag_validate(cursor, end, &skip_length, NULL)
          || engine_text_icon_tag_validate(cursor, end, &skip_length, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (engine_text_color_tag_validate(cursor, end, &skip_length, NULL)
                 || engine_text_icon_tag_validate(
                     cursor, end, &skip_length, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      }
      // Fall through to handle as normal character if it's not a valid tag
    }
    default:
      SDL_Log("character '%c'", *cursor);
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      count++;
      cursor++;
      break;
    }
  }

  return count;
}

engine_vec2_t
engine_text_measure(const engine_font_t *font, const char *rich_str)
{
  SDL_assert(font);

  if (!rich_str) {
    return (engine_vec2_t){ .x = 0, .y = 0 };
  }

  engine_vec2_t icon_range = engine_font_get_icon_range(font);
  engine_vec2_t char_range = engine_font_get_char_range(font);

  int line_spacing = engine_font_get_line_spacing(font);
  int char_spacing = engine_font_get_char_spacing(font);

  char base_char = engine_font_get_base(font);

  engine_vec2_t measure = { .x = 0, .y = 0 };

  measure.y = line_spacing;

  char *cursor = (char *)rich_str;
  char *end    = cursor + SDL_strlen(rich_str);

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      measure.y += line_spacing;
      cursor++;
      break;
    case '{': {
      int skip_length = 0;
      int icon_index;

      if (engine_text_color_tag_validate(cursor, end, &skip_length, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (engine_text_icon_tag_validate(
                     cursor, end, &skip_length, &icon_index)) {
        if (icon_index < icon_range.x || icon_index > icon_range.y) {
          SDL_LogWarn(
              SDL_LOG_CATEGORY_APPLICATION,
              "Invalid icon index %d in text string. Valid icon indices are "
              "from %d to %d.",
              icon_index,
              (int)icon_range.x,
              (int)icon_range.y);
          cursor += skip_length; // Skip the invalid tag
          break;
        }

        engine_rect_t icon_rect = engine_font_get_glyph_rect(font, icon_index);
        measure.x += (icon_rect.w + char_spacing);

        cursor += skip_length; // Skip the valid tag
        break;
      }
      // Fall through to handle as normal character if it's not a valid tag
    }
    default: {
      int glyphs_index = (int)*cursor - (int)base_char;
      if (glyphs_index < char_range.x || glyphs_index > char_range.y) {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Invalid character '%c' in text string. Valid characters are "
            "from %d to %d.",
            *cursor,
            (int)char_range.x,
            (int)char_range.y);
        cursor++;
        continue;
      }

      engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);
      measure.x += (glyph_rect.w + line_spacing);
      cursor++;
    } break;
    }
  }

  return measure;
}

char *
engine_text_wrap(const engine_font_t *font,
                 const char *rich_str,
                 float max_width,
                 float *out_height)
{
  SDL_assert(font);

  if (!rich_str) {
    if (out_height) {
      *out_height = 0;
    }
    return NULL;
  }

  // Create buffer
  size_t buffer_size  = 256;
  size_t buffer_count = 0;

  char *wrapped_text = NULL;
  ENGINE_ALLOC(wrapped_text, buffer_size);
  if (!wrapped_text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for wrapped text");
    // TODO add error
    return NULL;
  }

  int char_spacing = engine_font_get_char_spacing(font);

  char base_char = engine_font_get_base(font);

  engine_vec2_t icon_range = engine_font_get_icon_range(font);

  int width = 0;

  char *cursor = (char *)rich_str;
  char *end    = cursor + SDL_strlen(rich_str);

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      width = 0;
      break;
    case '{': {
      int skip_length = 0;
      int icon_index;

      if (engine_text_color_tag_validate(cursor, end, &skip_length, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (engine_text_icon_tag_validate(
                     cursor, end, &skip_length, &icon_index)) {
        if (icon_index < icon_range.x || icon_index > icon_range.y) {
          SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                      "Invalid icon index %d in text string. Valid icon "
                      "indices are from %d to %d.",
                      icon_index,
                      (int)icon_range.x,
                      (int)icon_range.y);
          cursor += skip_length; // Skip the invalid tag
          break;
        }

        engine_rect_t icon_rect = engine_font_get_glyph_rect(font, icon_index);

        int next_width = width + icon_rect.w + char_spacing; // Calculate the
                                                             // width if we add
                                                             // the next icon

        // Add a line break if adding the next icon would exceed the
        // maximum width
        if (next_width > max_width) {
          //  Check if we need to reallocate the buffer before adding
          //  the current character
          if (buffer_count + 1 >= buffer_size) {
            goto realloc;
          }

          wrapped_text[buffer_count++] = '\n';
          width                        = 0;
        }

        width += (icon_rect.w + char_spacing);

        cursor += skip_length; // Skip the valid tag
        break;
      }
      // Fall through to handle as normal character if it's not a valid tag
    }
    default: {
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      int glyphs_index         = (int)*cursor - (int)base_char;
      engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

      int next_width
          = width + glyph_rect.w + char_spacing; // Calculate the width if we
                                                 // add the next character

      // Add a line break if adding the next character would exceed the
      // maximum width
      if (next_width > max_width) {
        //  Check if we need to reallocate the buffer before adding the
        //  current
        if (buffer_count + 1 >= buffer_size) {
          goto realloc;
        }

        wrapped_text[buffer_count++] = '\n';
        width                        = 0;
      }

      //  Check if we need to reallocate the buffer before adding the current
      //  character
      if (buffer_count + 1 >= buffer_size) {
        goto realloc;
      }

      wrapped_text[buffer_count++] = *cursor;
      width                        = next_width;

      cursor++;
    } break;
    }

    continue; // Skip the realloc part

  realloc:
    // If passed here, the cursor was not incremented, so we need to process
    // the same character again after reallocating.
    buffer_size *= 2;

    char *new_buffer = NULL;
    ENGINE_ALLOC(new_buffer, buffer_size);
    if (!new_buffer) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Failed to allocate memory for wrapped text");
      // TODO add error
      ENGINE_FREE(wrapped_text);
      return NULL;
    }

    SDL_memcpy(new_buffer, wrapped_text, buffer_count);
    ENGINE_FREE(wrapped_text);

    wrapped_text = new_buffer;
  }

  return NULL;
}
