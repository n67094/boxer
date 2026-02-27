#include <SDL3/SDL.h>

#include "engine_font.h"
#include "engine_image.h"
#include "engine_mem.h"
#include "engine_painter.h"

struct engine_font_s
{
  engine_image_t image;
  engine_rect_t *glyphs;
  size_t glyph_count;
  int tab_size;
  int char_spacing;
  int line_spacing;
};

static void
engine_font_skip_color_tag(char *cursor)
{
  if (*cursor + 1 == 'c' && *cursor + 12 == '}') {
    // Skip opening tag
    *cursor += 2;
    while (*cursor && *cursor != '}') {
      cursor++;
    }
    cursor++;
  } else if (*cursor + 1 == '/' && *cursor + 2 == 'c' && *cursor + 3 == '}') {
    // Skip the closing tag
    *cursor += 4;
  }
}

static engine_color_t
engine_font_handle_color_tag(char *cursor)
{
  cursor += 3; // Skip the "{c=" part

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

static int
engine_font_handle_icon_tag(const engine_font_t *font, char *cursor)
{
  cursor += 3; // Skip the "{i=" part

  char icon_index_str[16];

  for (int i = 0; *cursor && *cursor != '}'; ++i) {
    icon_index_str[i]     = *cursor;
    icon_index_str[i + 1] = '\0';
    cursor++;
  }

  cursor++; // Skip the closing '}'

  int icon_index = SDL_strtol(icon_index_str, NULL, 10);

  if (icon_index < 224) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Invalid glyph index: %d. Icon indices must be 224 or higher.",
                 icon_index);
    return -1;
  }

  return icon_index;
}

engine_font_t *
engine_font_atlas_load(const char *path,
                       engine_rect_t *glyphs,
                       size_t glyph_count,
                       int char_spacing,
                       int line_spacing)
{
  SDL_assert(path);
  SDL_assert(glyphs);
  SDL_assert(glyph_count > 0);

  engine_font_t *font = NULL;
  ENGINE_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for atlas font");
    return NULL;
  }

  font->image        = engine_image_load(path);
  font->glyphs       = glyphs;
  font->glyph_count  = glyph_count;
  font->char_spacing = char_spacing;
  font->line_spacing = line_spacing;

  return font;
}

engine_font_t *
engine_font_atlas_mem(unsigned int width,
                      unsigned int height,
                      const void *data,
                      engine_rect_t *glyphs,
                      size_t glyph_count,
                      int char_spacing,
                      int line_spacing)
{
  SDL_assert(data);
  SDL_assert(width > 0);
  SDL_assert(height > 0);
  SDL_assert(glyphs);
  SDL_assert(glyph_count > 0);

  engine_font_t *font = NULL;
  ENGINE_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for atlas font");
    return NULL;
  }

  font->image        = engine_image_load_mem(width, height, (void *)data);
  font->glyphs       = glyphs;
  font->glyph_count  = glyph_count;
  font->char_spacing = char_spacing;
  font->line_spacing = line_spacing;

  return font;
}

void
engine_font_destroy(engine_font_t *font)
{
  if (!font)
    engine_image_destroy(font->image);
}

int
engine_font_get_char_spacing(const engine_font_t *font)
{
  SDL_assert(font);
  return font->char_spacing;
}

void
engine_font_set_char_spacing(engine_font_t *font, int spacing)
{
  SDL_assert(font);
  font->char_spacing = spacing;
}

int
engine_font_get_line_spacing(const engine_font_t *font)
{
  SDL_assert(font);
  return font->line_spacing;
}

void
engine_font_set_line_spacing(engine_font_t *font, int line_spacing)
{
  SDL_assert(font);
  font->line_spacing = line_spacing;
}

engine_vec2_t
engine_font_measure_text(const engine_font_t *font, const char *text)
{
  SDL_assert(font);

  if (!text) {
    return (engine_vec2_t){ .x = 0, .y = 0 };
  }

  engine_vec2_t retval = { .x = 0, .y = 0 };

  retval.y = font->line_spacing;

  char *cursor = (char *)text;

  while (*cursor) {
    int glyphs_index = (int)*cursor - ' ';

    switch (*cursor) {
    case '\n':
      retval.y += font->line_spacing;
      cursor++;
      break;
    case '{':
      if (*cursor + 1 == 'c' && *cursor + 12 == '}') {
        // Handle for color tags
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'c'
                 && *cursor + 3 == '}') {
        // Handle for closing color tags
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        // Handle for icon tag
        int icon_index = engine_font_handle_icon_tag(font, cursor);
        if (icon_index >= 0) {
          retval.x += (font->glyphs[icon_index - ' '].w + font->char_spacing);
        }
      }
      break;
    default:
      // Skip invalid characters
      if (*cursor < ' ' || *cursor > ' ' + font->glyph_count) {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Invalid character '%c' found while measure text. Only characters "
            "from ' ' (space) to '%c' are supported (Skipping the invalid "
            "character).",
            *cursor,
            (char)(' ' + font->glyph_count - 1));
        cursor++;
        continue;
      }

      retval.x += (font->glyphs[glyphs_index].w + font->char_spacing);
      cursor++;
    }
  }

  return retval;
}

char *
engine_font_wrap_text(const engine_font_t *font,
                      const char *text,
                      float max_width,
                      float *out_height)
{
  SDL_assert(font);

  if (!text) {
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
    return NULL;
  }

  int width = 0;

  char *cursor = (char *)text;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      width = 0;
      break;
    case '{':
      if (*cursor + 1 == 'c' && *cursor + 12 == '}') {
        // Check for color tags
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'c'
                 && *cursor + 3 == '}') {
        // Check for closing color tags
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        // Check for icon tags
        cursor += 3; // Skip the "{i=" part
        int icon_index = engine_font_handle_icon_tag(font, cursor);
        if (icon_index >= 0) {
          width += (font->glyphs[icon_index - ' '].w + font->char_spacing);
        }
      }
      break;
    default: {
      // Skip invalid characters
      if (*cursor < ' ' || *cursor > ' ' + font->glyph_count) {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Invalid character '%c' found while measure text. Only characters "
            "from ' ' (space) to '%c' are supported (Skipping the invalid "
            "character).",
            *cursor,
            (char)(' ' + font->glyph_count - 1));
        cursor++;
        continue;
      }

      int glyphs_index = (int)*cursor - ' ';

      int next_width
          = width + font->glyphs[glyphs_index].w + font->char_spacing;

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
      ENGINE_FREE(wrapped_text);
      return NULL;
    }

    SDL_memcpy(new_buffer, wrapped_text, buffer_count);
    ENGINE_FREE(wrapped_text);

    wrapped_text = new_buffer;
  }

  return NULL;
}

void
engine_font_render_char(engine_vec2_t position,
                        const engine_font_t *font,
                        char c,
                        engine_color_t background,
                        engine_color_t foreground)
{
  SDL_assert(font);

  if (c < ' ' || c > ' ' + font->glyph_count) {
    SDL_LogWarn(
        SDL_LOG_CATEGORY_APPLICATION,
        "Invalid character '%c' found while rendering text. Only characters "
        "from ' ' (space) to '%c' are supported (Skipping the invalid "
        "character).",
        c,
        (char)(' ' + font->glyph_count - 1));
    return;
  }

  int glyphs_index = (int)c - ' ';

  engine_rect_t glyph_rect = font->glyphs[glyphs_index];

  engine_rect_t dest_rect = {
    .x = position.x, .y = position.y, .w = glyph_rect.w, .h = glyph_rect.h
  };
  engine_rect_t src_rect = {
    .x = glyph_rect.x, .y = glyph_rect.y, .w = glyph_rect.w, .h = glyph_rect.h
  };

  engine_painter_set_color(
      background.r, background.g, background.b, background.a);
  engine_painter_draw_filled_rect(
      dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);

  engine_painter_set_image(font->image);
  engine_painter_set_color(
      foreground.r, foreground.g, foreground.b, foreground.a);

  engine_painter_draw_textured_rect(dest_rect, src_rect);
}

void
engine_font_render_text(engine_vec2_t position,
                        const engine_font_t *font,
                        const char *text,
                        engine_color_t background,
                        engine_color_t foreground)
{
  SDL_assert(font);
  if (!text) {
    return;
  }

  engine_color_t current_color = foreground;

  char *cursor = (char *)text;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      position.y += (font->line_spacing);
      position.x = 0;
      cursor++;
      break;
    case '{':
      if (*cursor + 1 == 'c' && *cursor + 12 == '}') {
        // Handle color tags
        current_color = engine_font_handle_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'c'
                 && *cursor + 3 == '}') {
        // Handle closing color tags
        current_color = foreground;
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        // Handle icon tags
        int icon_index = engine_font_handle_icon_tag(font, cursor);

        if (icon_index >= 0) {
          engine_font_render_char(
              position, font, (char)icon_index, background, current_color);
        }
      }
    default:
      // Skip invalid characters
      if (*cursor < ' ' || *cursor > ' ' + font->glyph_count) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "Invalid character '%c' found while rendering text. Only "
                    "characters from ' ' (space) to '%c' are supported "
                    "(Skipping the invalid character).",
                    *cursor,
                    (char)(' ' + font->glyph_count - 1));
        cursor++;
        continue;
      }

      engine_font_render_char(
          position, font, *cursor, background, current_color);

      position.x += (font->glyphs[*cursor - ' '].w + font->char_spacing);
      cursor++;
    }
  }
}
