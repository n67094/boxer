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
    cursor++;
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

  if (icon_index < font->glyph_spacing_index) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Invalid glyph index: %d. Icon indices must be 224 or higher.",
                icon_index);
    return -1;
  }

  return icon_index;
}

engine_text_t *
engine_tex_glyph(engine_vec2_t position,
                 const engine_font_t *font,
                 int glyphs_index,
                 engine_color_t background,
                 engine_color_t foreground)
{
  SDL_assert(font);
  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < font->glyph_count);

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

  text->entries[0].dest_rect = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = font->glyphs[glyphs_index].w,
    .h = font->glyphs[glyphs_index].h,
  };
  text->entries[0].src_rect   = &font->glyphs[glyphs_index];
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;

  return text;

  /*
  SDL_assert(font);
  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < font->glyph_count);

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
  */
}

engine_text_t *
engine_text_char(engine_vec2_t position,
                 const engine_font_t *font,
                 char c,
                 engine_color_t background,
                 engine_color_t foreground)
{
  SDL_assert(font);

  if (c < ' ') {
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

  text->entries[0].dest_rect = (engine_rect_t){
    .x = position.x,
    .y = position.y,
    .w = font->glyphs[c - font->glyph_spacing_index].w,
    .h = font->glyphs[c - font->glyph_spacing_index].h,
  };
  text->entries[0].src_rect   = &font->glyphs[c - font->glyph_spacing_index];
  text->entries[0].background = background;
  text->entries[0].foreground = foreground;

  return text;
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

  size_t text_length = engine_text_length(text);

  engine_text_t *retval = NULL;
  ENGINE_ALLOC(retval, text_length * sizeof(engine_text_t));
  if (!text) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text");
    // TODO add error
    return;
  }

  retval->entry_count = text_length;
  ENGINE_ALLOC(retval->entries, retval->entry_count);
  if (!retval->entries) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for rendered text entries");
    // TODO add error
    ENGINE_FREE(retval);
    return;
  }

  engine_color_t cur_foreground = foreground;
  engine_color_t cur_background = background;

  char *cursor = (char *)text;

  int entry_index = 0;

  while (*cursor) {
    switch (*cursor) {
    case '\n':
      position.y += (font->line_spacing);
      position.x = 0;
      cursor++;
      break;
    case '{':
      if (*cursor + 1 == 'b' && *cursor + 12 == '}') { // Background
        // Handle color tags
        cur_background = engine_text_handle_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'b'
                 && *cursor + 3 == '}') {
        // Handle closing color tags
        cur_background = background;
      } else if (*cursor + 1 == 'f' && *cursor + 12 == '}') { // Foreground
        // Handle color tags
        cur_foreground = engine_text_handle_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'f'
                 && *cursor + 3 == '}') {
        // Handle closing color tags
        cur_foreground = foreground;
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') { // Icon
        // Handle icon tags
        int icon_index = engine_text_handle_icon_tag(font, cursor);

        if (icon_index >= 0) {
          retval->entries[entry_index++] = (engine_text_entry_t){
            .dest_rect  = (engine_rect_t){
              .x = position.x,
              .y = position.y,
              .w = font->glyphs[icon_index].w,
              .h = font->glyphs[icon_index].h,
            },
            .src_rect   = &font->glyphs[icon_index],
            .background = cur_background,
            .foreground = cur_foreground,
          };
        }
      }
      break;
    default:
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      retval->entries[entry_index++] = (engine_text_entry_t){
        .dest_rect  = (engine_rect_t){
          .x = position.x,
          .y = position.y,
          .w = font->glyphs[*cursor - font->glyph_spacing_index].w,
          .h = font->glyphs[*cursor - font->glyph_spacing_index].h,
        },
        .src_rect   = &font->glyphs[*cursor - font->glyph_spacing_index],
        .background = cur_background,
        .foreground = cur_foreground,
      };

      position.x += (font->glyphs[*cursor - font->glyph_spacing_index].w
                     + font->char_spacing);
      cursor++;
    }
  }
}

engine_vec2_t
engine_text_measure(const engine_font_t *font, const char *text)
{
  SDL_assert(font);

  if (!text) {
    return (engine_vec2_t){ .x = 0, .y = 0 };
  }

  engine_vec2_t retval = { .x = 0, .y = 0 };

  retval.y = font->line_spacing;

  char *cursor = (char *)text;

  while (*cursor) {
    int glyphs_index = (int)*cursor - font->glyph_spacing_index;

    switch (*cursor) {
    case '\n':
      retval.y += font->line_spacing;
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

        if (icon_index >= 0) {
          retval.x += (font->glyphs[icon_index].w + font->char_spacing);
        }
      }
      break;
    default:
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      retval.x += (font->glyphs[glyphs_index].w + font->char_spacing);
      cursor++;
    }
  }

  return retval;
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
engine_text_wrap(const engine_font_t *font,
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
    // TODO add error
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

        if (icon_index >= 0) {
          width += (font->glyphs[icon_index].w + font->char_spacing);
        }
      }
      break;
    default: {
      // Discard invalid characters
      if (*cursor < ' ') {
        cursor++;
        continue;
      }

      int glyphs_index = (int)*cursor - font->glyph_spacing_index;

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
