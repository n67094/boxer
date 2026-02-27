#include <SDL3/SDL.h>

#include "engine_font.h"
#include "engine_image.h"
#include "engine_mem.h"

struct engine_font_s
{
  engine_image_t image;
  engine_rect_t *glyphs;
  size_t glyph_count;
  int tab_size;
  int char_spacing;
  int line_spacing;
};

engine_font_t *
engine_font_atlas_load(const char *path,
                       engine_rect_t *glyphs,
                       size_t glyph_count,
                       int tab_size,
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
  font->tab_size     = tab_size;
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
                      int tab_size,
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
  font->tab_size     = tab_size;
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
  char color_str[9]; // 8 characters for RGBA + null terminator

  for (int i = 0; i < 8 && cursor[i] && cursor[i] != '}'; ++i) {
    color_str[i]     = cursor[i];
    color_str[i + 1] = '\0';
  }

  Uint32 color_value   = SDL_strtoul(color_str, NULL, 16);
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
  char icon_index_str[16];

  for (int i = 0; *cursor && *cursor != '}'; ++i) {
    icon_index_str[i]     = *cursor;
    icon_index_str[i + 1] = '\0';
    cursor++;
  }

  int icon_index = SDL_strtol(icon_index_str, NULL, 10);

  if (icon_index < 224) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Invalid glyph index: %d. Icon indices must be 224 or higher.",
                 icon_index);
    return -1;
  }

  return icon_index;
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
    case '\t':
      retval.x += (font->glyphs[0].w + font->char_spacing) * font->tab_size;
      cursor++;
      break;
    case '{':
      if (*cursor + 1 == 'c' && *cursor + 12 == '}') {
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == '/' && *cursor + 2 == 'c'
                 && *cursor + 3 == '}') {
        engine_font_skip_color_tag(cursor);
      } else if (*cursor + 1 == 'i' && *cursor + 2 == '=') {
        cursor += 3; // Skip the "{i=" part
        int icon_index = engine_font_handle_icon_tag(font, cursor);
        if (icon_index >= 0) {
          retval.x += (font->glyphs[icon_index - ' '].w + font->char_spacing);
        }
        cursor++; // Skip the closing '}'
      }
      break;
    default:
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
                      float *out_height,
                      engine_font_align_e align)
{
  SDL_assert(font);

  if (!text) {
    if (out_height) {
      *out_height = 0;
    }
    SDL_strdup("");
  }

  return NULL;
}

void
engine_font_render(engine_vec2_t position,
                   const engine_font_t *font,
                   const char *text,
                   engine_color_t background,
                   engine_color_t foreground)
{
  SDL_assert(font);
  SDL_assert(text);
}

void engine_font_render_wrapped(engine_vec2_t position,
                                const engine_font_t *font,
                                const char *text,
                                float max_width,
                                engine_font_align_e align,
                                engine_color_t background,
                                engine_color_t foreground);
