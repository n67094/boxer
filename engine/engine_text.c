#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_font.h"
#include "engine_mem.h"
#include "engine_text.h"

static void
engine_text_discard_color_tag(char *cursor)
{
  if ((*cursor + 1 == 'b' || *cursor + 1 == 'f') && *cursor + 12 == '}') {
    // Discard opening tag
    *cursor += 2;
    while (*cursor && *cursor != '}') {
      cursor++;
    }

    if (*cursor == '\0') {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Unterminated color tag in text string.");
      return;
    }

    cursor++; // Discard the closing '}'
  } else if (*cursor + 1 == '/' && (*cursor + 2 == 'b' || *cursor + 2 == 'f')
             && *cursor + 3 == '}') {
    // Discard the closing tag
    *cursor += 4;
  }
}

static engine_color_t
engine_text_handle_color_tag(char *cursor)
{
  cursor += 3; // Discard the "{c=" part

  char color_str[9]; // 8 characters for RGBA + null terminator

  for (int i = 0; i < 8 && cursor[i] && cursor[i] != '}'; ++i) {
    color_str[i]     = cursor[i];
    color_str[i + 1] = '\0';
  }

  Uint32 color_value = SDL_strtoul(color_str, NULL, 16);

  engine_color_t color = {
    .r = (color_value >> 24) & 0xFF,
    .g = (color_value >> 16) & 0xFF,
    .b = (color_value >> 8) & 0xFF,
    .a = color_value & 0xFF,
  };

  return color;
}

static void
engine_text_discard_icon_tag(char *cursor)
{
  if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
    cursor += 3; // Discard the "{i=" part

    while (*cursor && *cursor != '}') {
      cursor++;
    }

    if (*cursor == '\0') {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Unterminated icon tag in text string.");
      return;
    }

    cursor++; // Discard the closing '}'
  }
}

static int
engine_text_handle_icon_tag(const engine_font_t *font, char *cursor)
{
  cursor += 3; // Discard the "{i=" part

  char icon_index_str[16];

  for (int i = 0; *cursor && *cursor != '}'; ++i) {
    icon_index_str[i]     = *cursor;
    icon_index_str[i + 1] = '\0';
    cursor++;
  }

  cursor++; // Discard the closing '}'

  int icon_index = SDL_strtol(icon_index_str, NULL, 10);

  return icon_index;
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

  text->entries[0].dest_rect = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].src_rect   = glyph_rect;
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

  char base_char = engine_font_get_base_char(font);

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

  text->entries[0].dest_rect = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = glyph_rect.w,
    .h = glyph_rect.h,
  };
  text->entries[0].src_rect   = glyph_rect;
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

  engine_vec2_t char_range = engine_font_get_char_range(font);
  engine_vec2_t icon_range = engine_font_get_icon_range(font);

  int line_spacing = engine_font_get_line_spacing(font);
  int char_spacing = engine_font_get_char_spacing(font);

  int entry_index = 0;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      position.y += (line_spacing);
      position.x = 0;
      cursor++;
      break;
    case '{':
      if (*cursor + 1 == 'b' && *cursor + 12 == '}') { // Background
        // Handle color tags
        current_background = engine_text_handle_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'b'
                 && *cursor + 3 == '}') {
        // Handle closing color tags
        current_background = background;
      } else if (*cursor + 1 == 'f' && *cursor + 12 == '}') { // Foreground
        // Handle color tags
        current_foreground = engine_text_handle_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'f'
                 && *cursor + 3 == '}') {
        // Handle closing color tags
        current_foreground = foreground;
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') { // Icon
        // Handle icon tags
        int icon_index = engine_text_handle_icon_tag(font, cursor);

        if (icon_index < icon_range.x || icon_index > icon_range.y) {
          SDL_LogWarn(
              SDL_LOG_CATEGORY_APPLICATION,
              "Invalid icon index %d in text string. Valid icon indices are "
              "from %d to %d.",
              icon_index,
              (int)icon_range.x,
              (int)icon_range.y);
          continue;
        }

        engine_rect_t icon_rect = engine_font_get_glyph_rect(font, icon_index);

        text->entries[entry_index++] = (engine_text_entry_t){
            .dest_rect  = (engine_rect_t){
              .x = position.x,
              .y = position.y,
              .w = icon_rect.w,
              .h = icon_rect.h,
            },
            .src_rect   = icon_rect,
            .background = current_background,
            .foreground = current_foreground,
          };
      }
      break;
    default: {
      int glyphs_index = (int)*cursor - (int)engine_font_get_base_char(font);

      if (glyphs_index >= icon_range.x && glyphs_index <= icon_range.y) {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Invalid character '%c' in text string. It is reserved for icons. "
            "Use {i=%d} to render it as an icon.",
            *cursor,
            (int)*cursor);
        cursor++;
        continue;
      }

      engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

      text->entries[entry_index++] = (engine_text_entry_t){
        .dest_rect  = (engine_rect_t){
          .x = position.x,
          .y = position.y,
          .w = glyph_rect.w,
          .h = glyph_rect.h,
        },
        .src_rect   = glyph_rect,
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

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      buffer[buffer_count++] = '\n';
      cursor++;
      break;
    case '{':
      if ((*cursor + 1 == 'b' || *cursor + 1 == 'f') && *cursor + 12 == '}') {
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == '/'
                 && (*cursor + 2 == 'b' || *cursor + 2 == 'f')
                 && *cursor + 3 == '}') {
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        engine_text_discard_icon_tag(cursor);
      }
      break;
    default:
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      buffer[buffer_count++] = *cursor;
      cursor++;
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

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      count++;
      cursor++;
      break;
    case '{':
      if ((*cursor + 1 == 'b' || *cursor + 1 == 'f') && *cursor + 12 == '}') {
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == '/'
                 && (*cursor + 2 == 'b' || *cursor + 2 == 'f')
                 && *cursor + 3 == '}') {
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        engine_text_discard_icon_tag(cursor);
        count++;
        cursor++;
      }
      break;
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

  char base_char = engine_font_get_base_char(font);

  engine_vec2_t measure = { .x = 0, .y = 0 };

  measure.y = line_spacing;

  char *cursor = (char *)rich_str;

  while (*cursor) {
    int glyphs_index = (int)*cursor - (int)base_char;

    switch (*cursor) {
    case '\n':
      measure.y += line_spacing;
      cursor++;
      break;
    case '{':
      if ((*cursor + 1 == 'c' || *cursor + 1 == 'f') && *cursor + 12 == '}') {
        // Handle for color tags
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == '/'
                 && (*cursor + 2 == 'c' || *cursor + 2 == 'f')
                 && *cursor + 3 == '}') {
        // Handle for closing color tags
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        // Handle for icon tag
        int icon_index = engine_text_handle_icon_tag(font, cursor);
        if (icon_index >= icon_range.x && icon_index <= icon_range.y) {
          engine_rect_t icon_rect
              = engine_font_get_glyph_rect(font, icon_index);

          measure.x += (icon_rect.w + char_spacing);
        }
      }
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

  char base_char = engine_font_get_base_char(font);

  engine_vec2_t icon_range = engine_font_get_icon_range(font);

  int width = 0;

  char *cursor = (char *)rich_str;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      width = 0;
      break;
    case '{':
      if ((*cursor + 1 == 'b' || *cursor + 1 == 'f') && *cursor + 12 == '}') {
        // Check for color tags
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == '/'
                 && (*cursor + 2 == 'b' || *cursor + 2 == 'f')
                 && *cursor + 3 == '}') {
        // Check for closing color tags
        engine_text_discard_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        // Check for icon tags
        int icon_index = engine_text_handle_icon_tag(font, cursor);

        engine_rect_t icon_rect = engine_font_get_glyph_rect(font, icon_index);

        if (icon_index >= icon_range.x && icon_index <= icon_range.y) {
          width += (icon_rect.w + char_spacing);
        }
      }
      break;
    default: {
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      int glyphs_index         = (int)*cursor - (int)base_char;
      engine_rect_t glyph_rect = engine_font_get_glyph_rect(font, glyphs_index);

      int next_width
          = width + glyph_rect.w
            + char_spacing; // Calculate the width if we add the next character

      // Add a line break if adding the next character would exceed the maximum
      // width
      if (next_width > max_width) {
        //  Check if we need to reallocate the buffer before adding the current
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
    // If passed here, the cursor was not incremented, so we need to process the
    // same character again after reallocating.
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
