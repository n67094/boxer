#include <SDL3/SDL.h>

#include "bxr_font.h"
#include "bxr_image.h"
#include "bxr_mem.h"

struct bxr_font_s
{
  bxr_image_t image;
  const bxr_rect_t *glyphs;
  size_t glyph_count;
  bxr_vec2_t icon_range;
  bxr_vec2_t char_range;
  int base; // The value to subtract from a char to get the corresponding glyph
            // index in the font.
  int char_spacing;
  int line_spacing;
};

bxr_font_t *
bxr_font_load(const char *path,
              const bxr_rect_t *glyphs,
              size_t glyph_count,
              bxr_vec2_t icon_range,
              bxr_vec2_t char_range,
              char first_char,
              int char_spacing,
              int line_spacing)
{
  SDL_assert(path);
  SDL_assert(glyphs);
  SDL_assert(glyph_count > 0);

  bxr_font_t *font = NULL;
  BXR_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for atlas font");
    return NULL;
  }

  font->image        = bxr_image_load(path);
  font->glyphs       = glyphs;
  font->glyph_count  = glyph_count;
  font->icon_range   = icon_range;
  font->char_range   = char_range;
  font->base         = first_char - (char)char_range.x;
  font->char_spacing = char_spacing;
  font->line_spacing = line_spacing;

  return font;
}

bxr_font_t *
bxr_font_mem(unsigned int width,
             unsigned int height,
             const void *data,
             const bxr_rect_t *glyphs,
             size_t glyph_count,
             bxr_vec2_t icon_range,
             bxr_vec2_t char_range,
             char first_char,
             int char_spacing,
             int line_spacing)
{
  SDL_assert(data);
  SDL_assert(width > 0);
  SDL_assert(height > 0);
  SDL_assert(glyphs);
  SDL_assert(glyph_count > 0);

  bxr_font_t *font = NULL;
  BXR_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for atlas font");
    return NULL;
  }

  font->image        = bxr_image_load_mem(width, height, (void *)data);
  font->glyphs       = glyphs;
  font->glyph_count  = glyph_count;
  font->icon_range   = icon_range;
  font->char_range   = char_range;
  font->base         = first_char - (char)char_range.x;
  font->char_spacing = char_spacing;
  font->line_spacing = line_spacing;

  return font;
}

bxr_image_t
bxr_font_get_image(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->image;
}

void
bxr_font_set_char_spacing(bxr_font_t *font, int spacing)
{
  SDL_assert(font);
  font->char_spacing = spacing;
}

bxr_vec2_t
bxr_font_get_icon_range(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->icon_range;
}

bxr_vec2_t
bxr_font_get_char_range(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->char_range;
}

int
bxr_font_get_glyph_count(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->glyph_count;
}

bxr_rect_t
bxr_font_get_glyph_rect(const bxr_font_t *font, int glyphs_index)
{
  SDL_assert(font);

  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < font->glyph_count);
  return font->glyphs[glyphs_index];
}

void
bxr_font_destroy(bxr_font_t *font)
{
  if (!font) {
    bxr_image_destroy(font->image);
    BXR_FREE(font);
  }
}

int
bxr_font_get_base(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->base;
}

int
bxr_font_get_char_spacing(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->char_spacing;
}

int
bxr_font_get_line_spacing(const bxr_font_t *font)
{
  SDL_assert(font);
  return font->line_spacing;
}

void
bxr_font_set_line_spacing(bxr_font_t *font, int line_spacing)
{
  SDL_assert(font);
  font->line_spacing = line_spacing;
}
