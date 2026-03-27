#include <SDL3/SDL.h>

#include "bxr_font.h"
#include "bxr_math.h"
#include "bxr_text.h"

bxr_vec2_t
bxr_text_measure(const bxr_font_t *font, const char *str)
{
  SDL_assert(font);

  if (str == NULL || *str == '\0') {
    return (bxr_vec2_t){ 0, 0 };
  }

  char base_char        = bxr_font_get_base(font);
  int line_spacing      = bxr_font_get_line_spacing(font);
  int char_spacing      = bxr_font_get_char_spacing(font);
  bxr_vec2_t char_range = bxr_font_get_char_range(font);

  bxr_vec2_t measure = { 0, 0 };

  char *cursor = (char *)str;

  while (*cursor) {

    switch (*cursor) {
    case '\n':
      measure.y += line_spacing;
      cursor++;
      break;
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
      bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);
      measure.x += (glyph_rect.w + char_spacing);
      cursor++;
    } break;
    }
  }

  return measure;
}

/* FIXME what to do with this ? Trash I guess
#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_font.h"
#include "bxr_mem.h"
#include "bxr_text.h"

#define BXR_TEXT_TAG_COLOR_OPEN_LENGTH 12 // Length of "{X=XXXXXXXX}"
#define BXR_TEXT_TAG_COLOR_CLOSE_LENGTH 4 // Length of "{/X}"
#define BXR_TEXT_TAG_ICON_LENGTH 6        // Length of "{X=XX}"

typedef enum
{
  BXR_TEXT_TAG_NONE = 0,
  BXR_TEXT_TAG_OPEN_COLOR_BACKGROUND,
  BXR_TEXT_TAG_CLOSE_COLOR_BACKGROUND,
  BXR_TEXT_TAG_OPEN_COLOR_FOREGROUND,
  BXR_TEXT_TAG_CLOSE_COLOR_FOREGROUND,
  BXR_TEXT_TAG_ICON,
} bxr_text_tag_e;

static bool
bxr_text_color_tag_validate(const char *cursor,
                               const char *end,
                               int *out_skip_length,
                               bxr_color_t *out_color,
                               bxr_text_tag_e *out_tag_type)
{
  if (*(cursor + 1) != 'b' && *(cursor + 1) != 'f' && *(cursor + 1) != '/') {
    return false;
  }

  // Opening tag: {b=RRGGBBAA} or {f=RRGGBBAA}
  if (*(cursor + 1) == 'b' || *(cursor + 1) == 'f') {
    if (out_tag_type) {
      *out_tag_type = (*(cursor + 1) == 'b')
                          ? BXR_TEXT_TAG_OPEN_COLOR_BACKGROUND
                          : BXR_TEXT_TAG_OPEN_COLOR_FOREGROUND;
    }

    if (*(cursor + 2) != '=') {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Missing '=' in color tag.");
      return false;
    }

    if (cursor + BXR_TEXT_TAG_COLOR_OPEN_LENGTH > end) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Invalid color tag length.");
      return false;
    }

    if (*(cursor + BXR_TEXT_TAG_COLOR_OPEN_LENGTH - 1) != '}') {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Missing closing '}' in color tag.");
      return false;
    }

    if (out_skip_length) {
      *out_skip_length = BXR_TEXT_TAG_COLOR_OPEN_LENGTH;
    }

    if (out_color == NULL) {
      return true; // Only validating the tag format, not extracting color
    }

    size_t color_str_len = 9; // 8 characters for RGBA + null terminator
    char color_str[color_str_len];

    int i;
    for (i = 0; i < color_str_len - 1 && cursor[i] != '}'; ++i) {
      char c       = cursor[3 + i]; // Start after "{X="
      color_str[i] = c;
    }
    color_str[i] = '\0';

    char *endptr       = NULL;
    Uint32 color_value = SDL_strtoul(color_str, &endptr, 16);

    if (endptr != color_str + color_str_len - 1) {
      SDL_LogWarn(
          SDL_LOG_CATEGORY_APPLICATION,
          "Invalid color value in color tag. Expected a hexadecimal number.");
      return false;
    }

    *out_color = (bxr_color_t){
      .r = (color_value >> 24) & 0xFF,
      .g = (color_value >> 16) & 0xFF,
      .b = (color_value >> 8) & 0xFF,
      .a = color_value & 0xFF,
    };

    return true;
  }

  // Closing tag: {/b} or {/f}
  if (*(cursor + 1) == '/' && (*(cursor + 2) == 'b' || *(cursor + 2) == 'f'))
{ if (out_tag_type) { *out_tag_type = (*(cursor + 2) == 'b') ?
BXR_TEXT_TAG_CLOSE_COLOR_BACKGROUND : BXR_TEXT_TAG_CLOSE_COLOR_FOREGROUND;
    }

    if (cursor + BXR_TEXT_TAG_COLOR_CLOSE_LENGTH > end) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Invalid color tag length.");
      return false;
    }

    if (*(cursor + BXR_TEXT_TAG_COLOR_CLOSE_LENGTH - 1) != '}') {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Missing closing '}' in color tag.");
      return false;
    }

    if (out_skip_length) {
      *out_skip_length = BXR_TEXT_TAG_COLOR_CLOSE_LENGTH;
    }

    return true;
  }

  return false;
}

static bool
bxr_text_icon_tag_validate(const char *cursor,
                              const char *end,
                              int *skip_length,
                              int *out_icon_index)
{
  if (*(cursor + 1) != 'i' || *(cursor + 2) != '=') {
    return false;
  }

  if (cursor + BXR_TEXT_TAG_ICON_LENGTH > end) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Invalid icon tag length.");
    return false;
  }

  if (*(cursor + BXR_TEXT_TAG_ICON_LENGTH - 1) != '}') {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Missing closing '}' in icon tag.");
    return false;
  }

  if (skip_length) {
    *skip_length = BXR_TEXT_TAG_ICON_LENGTH;
  }

  char icon_str_len = 3; // 2 characters for icon index + null terminator
  char icon_str[icon_str_len];

  int i;
  for (i = 0; i < icon_str_len - 1 && cursor[i] != '}'; ++i) {
    char c      = cursor[3 + i]; // Start after "{i="
    icon_str[i] = c;
  }
  icon_str[i] = '\0';

  char *endptr   = NULL;
  int icon_index = SDL_strtoul(icon_str, &endptr, 16);

  if (endptr != icon_str + icon_str_len - 1) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Invalid icon index in icon tag. Expected a number.");
    return false;
  }

  if (out_icon_index) {
    *out_icon_index = icon_index;
  }

  return true;
}

bxr_text_t *
bxr_text_glyph_make(const bxr_font_t *font,
                       int glyphs_index,
                       bxr_color_t background,
                       bxr_color_t foreground)
{
  SDL_assert(font);

  size_t glyph_count = bxr_font_get_glyph_count(font);
  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < glyph_count);

  bxr_text_t *text = NULL;
  BXR_NEW(text);
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text glyph");
    // TODO add error
    return NULL;
  }

  text->entry_count = 1;
  BXR_CALLOC(text->entries, text->entry_count, sizeof(bxr_text_entry_t));
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text glyph entries");
    // TODO add error
    BXR_FREE(text);
    return NULL;
  }

  bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);

  text->entries[0].rects.dst = (bxr_rect_t){
    .x = 0,
    .y = 0,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].rects.src  = glyph_rect;
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;
  text->font                  = font;

  return text;
}

bxr_text_t *
bxr_text_char_make(const bxr_font_t *font,
                      char c,
                      bxr_color_t background,
                      bxr_color_t foreground)
{
  SDL_assert(font);

  bxr_vec2_t char_range = bxr_font_get_char_range(font);

  char base_char = bxr_font_get_base(font);

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

  bxr_text_t *text = NULL;
  BXR_NEW(text);
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text char");
    // TODO add error
    return NULL;
  }

  text->entry_count = 1;
  BXR_CALLOC(text->entries, text->entry_count, sizeof(bxr_text_entry_t));
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for text char entries");
    // TODO add error
    BXR_FREE(text);
    return NULL;
  }

  bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);

  text->entries[0].rects.dst = (bxr_rect_t){
    .x = 0,
    .y = 0,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].rects.src  = glyph_rect;
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;
  text->font                  = font;

  return text;
}

bxr_text_t *
bxr_text_make(const bxr_font_t *font,
                 const char *rich_str,
                 bxr_color_t background,
                 bxr_color_t foreground)
{
  SDL_assert(font);
  if (!rich_str) {
    return NULL;
  }

  size_t rich_str_len              = SDL_strlen(rich_str);
  size_t rich_str_len_without_tags = bxr_text_length(rich_str);

  bxr_text_t *text = NULL;
  BXR_NEW(text);
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text");
    // TODO add error
    return NULL;
  }

  text->entry_count = rich_str_len_without_tags;
  BXR_CALLOC(text->entries, text->entry_count, sizeof(bxr_text_entry_t));
  if (!text->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text entries");
    // TODO add error
    BXR_FREE(text);
    return NULL;
  }

  int pos_x = 0;
  int pos_y = 0;

  bxr_color_t current_foreground = foreground;
  bxr_color_t current_background = background;

  char *cursor = (char *)rich_str;
  char *end    = cursor + rich_str_len;

  char base_char = bxr_font_get_base(font);

  bxr_vec2_t char_range = bxr_font_get_char_range(font);
  bxr_vec2_t icon_range = bxr_font_get_icon_range(font);

  int line_spacing = bxr_font_get_line_spacing(font);
  int char_spacing = bxr_font_get_char_spacing(font);

  int entry_index = 0;

  while (cursor < end && *cursor) {
    switch (*cursor) {
    case '\n':
      pos_y += (line_spacing);
      pos_x = 0;
      cursor++;
      break;
    case '{': {
      int skip_length            = 0;
      bxr_color_t new_color   = { 0 };
      bxr_text_tag_e tag_type = BXR_TEXT_TAG_NONE;
      int icon_index             = -1;

      // Handle color tags
      if (bxr_text_color_tag_validate(
              cursor, end, &skip_length, &new_color, &tag_type)) {
        switch (tag_type) {
        case BXR_TEXT_TAG_OPEN_COLOR_BACKGROUND:
          current_background = new_color;
          break;
        case BXR_TEXT_TAG_CLOSE_COLOR_BACKGROUND:
          current_background = background;
          break;

        case BXR_TEXT_TAG_OPEN_COLOR_FOREGROUND:
          current_foreground = new_color;
          break;
        case BXR_TEXT_TAG_CLOSE_COLOR_FOREGROUND:
          current_foreground = foreground;
          break;
        default:
          break;
        }

        cursor += skip_length;
        break;
      }
      // Handle icon tags
      else if (bxr_text_icon_tag_validate(
                   cursor, end, &skip_length, &icon_index)) {
        if (icon_index < icon_range.x || icon_index > icon_range.y) {
          SDL_LogWarn(
              SDL_LOG_CATEGORY_APPLICATION,
              "Invalid icon index %d in text string. Valid icon indices are "
              "from %d to %d.",
              icon_index,
              (int)icon_range.x,
              (int)icon_range.y);
          break;
        }

        bxr_rect_t icon_rect = bxr_font_get_glyph_rect(font, icon_index);

        text->entries[entry_index++] = (bxr_text_entry_t){
          .rects = (bxr_textured_rect_t){
            .dst  = (bxr_rect_t){
              .x = pos_x,
              .y = pos_y,
              .w = icon_rect.w,
              .h = icon_rect.h,
            },
            .src   = icon_rect,
          },
          .background = current_background,
          .foreground = current_foreground,
        };

        pos_x += icon_rect.w + char_spacing;

        cursor += skip_length;
        break;
      }
      // No break, Fall through to handle '{' as a normal character
    }
    default: {
      int glyphs_index = (int)*cursor - (int)base_char;

      if (glyphs_index < char_range.x || glyphs_index > char_range.y) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Invalid character '%c' in text string. Valid characters "
                    "are from %d to %d.",
                    *cursor,
                    (int)char_range.x,
                    (int)char_range.y);
        cursor++;
        continue;
      }

      bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);

      text->entries[entry_index++] = (bxr_text_entry_t){
        .rects     = (bxr_textured_rect_t){
          .dst  = (bxr_rect_t){
            .x = pos_x,
            .y = pos_y,
            .w = glyph_rect.w,
            .h = glyph_rect.h,
          },
          .src   = glyph_rect,
        },
        .background = current_background,
        .foreground = current_foreground,
      };

      pos_x += glyph_rect.w + char_spacing;
      cursor++;
    } break;
    }
  }

  return text;
}

void
bxr_text_destroy(bxr_text_t *text)
{
  if (text) {
    if (text->entries) {
      BXR_FREE(text->entries);
    }
    BXR_FREE(text);
  }
}

const bxr_text_entry_t *bxr_text_get_
{
  SDL_assert(text);
  return text->entries;
}

const bxr_textured_rect_t *
bxr_text_get_textured_rects(const bxr_text_t *text)
{
  SDL_assert(text);
  return (const bxr_textured_rect_t *)text->rects;
}

size_t
bxr_text_get_textured_rects_count(const bxr_text_t *text)
{
  SDL_assert(text);
  return text->rects_count;
}

char *
bxr_text_without_tags(const char *rich_str)
{
  SDL_assert(rich_str);

  // The length should be the same or shorter than the original text, since
  // we're discarding tags and invalid characters.
  size_t rich_str_len = bxr_text_length(rich_str);

  char *buffer     = NULL;
  int buffer_count = 0;
  BXR_ALLOC(buffer, rich_str_len + 1); // +1 for null terminator
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

      if (bxr_text_color_tag_validate(
              cursor, end, &skip_length, NULL, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (bxr_text_icon_tag_validate(
                     cursor, end, &skip_length, NULL)) {
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
bxr_text_length(const char *rich_str)
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
      int icon_index;

      if (bxr_text_color_tag_validate(
              cursor, end, &skip_length, NULL, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (bxr_text_icon_tag_validate(
                     cursor, end, &skip_length, &icon_index)) {
        count++;               // Count the icon as a single character
        cursor += skip_length; // Skip the valid tag
        break;
      }
      // Fall through to handle as normal character if it's not a valid tag
    }
    default:
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

  return count; // Subtract 1 to account for the null terminator
}

bxr_vec2_t
bxr_text_measure(const bxr_font_t *font, const char *rich_str)
{
  SDL_assert(font);

  if (!rich_str) {
    return (bxr_vec2_t){ .x = 0, .y = 0 };
  }

  bxr_vec2_t icon_range = bxr_font_get_icon_range(font);
  bxr_vec2_t char_range = bxr_font_get_char_range(font);

int line_spacing = bxr_font_get_line_spacing(font);
  int char_spacing = bxr_font_get_char_spacing(font);

  char base_char = bxr_font_get_base(font);

  bxr_vec2_t measure = { .x = 0, .y = 0 };

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

      if (bxr_text_color_tag_validate(
              cursor, end, &skip_length, NULL, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (bxr_text_icon_tag_validate(
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

        bxr_rect_t icon_rect = bxr_font_get_glyph_rect(font, icon_index);
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

      bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);
      measure.x += (glyph_rect.w + line_spacing);
      cursor++;
    } break;
    }
  }

  return measure;
}

char *
bxr_text_wrap(const bxr_font_t *font,
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
  BXR_ALLOC(wrapped_text, buffer_size);
  if (!wrapped_text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for wrapped text");
    // TODO add error
    return NULL;
  }

  int char_spacing = bxr_font_get_char_spacing(font);

  char base_char = bxr_font_get_base(font);

  bxr_vec2_t icon_range = bxr_font_get_icon_range(font);

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

      if (bxr_text_color_tag_validate(
              cursor, end, &skip_length, NULL, NULL)) {
        cursor += skip_length; // Skip the valid tag
        break;
      } else if (bxr_text_icon_tag_validate(
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

        bxr_rect_t icon_rect = bxr_font_get_glyph_rect(font, icon_index);

        int next_width = width + icon_rect.w + char_spacing; // Calculate the
                                                             // width if we
add
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
      bxr_rect_t glyph_rect = bxr_font_get_glyph_rect(font, glyphs_index);

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
    BXR_ALLOC(new_buffer, buffer_size);
    if (!new_buffer) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Failed to allocate memory for wrapped text");
      // TODO add error
      BXR_FREE(wrapped_text);
      return NULL;
    }

    SDL_memcpy(new_buffer, wrapped_text, buffer_count);
    BXR_FREE(wrapped_text);

    wrapped_text = new_buffer;
  }

  return NULL;
}
*/
