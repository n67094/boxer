#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_font.h"
#include "bxr_image.h"
#include "bxr_log.h"
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
bxr_font_create(const char *path,
                const bxr_rect_t *glyphs,
                size_t glyph_count,
                bxr_vec2_t icon_range,
                bxr_vec2_t char_range,
                char first_char,
                int char_spacing,
                int line_spacing)
{
  BXR_ASSERT(path);
  BXR_ASSERT(glyphs);
  BXR_ASSERT(glyph_count > 0);

  bxr_font_t *font = NULL;
  BXR_NEW(font);
  if (!font) {
    return NULL;
  }

  font->image        = bxr_image_create(path);
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
bxr_font_create_mem(unsigned int width,
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
  BXR_ASSERT(data);
  BXR_ASSERT(width > 0);
  BXR_ASSERT(height > 0);
  BXR_ASSERT(glyphs);
  BXR_ASSERT(glyph_count > 0);

  bxr_font_t *font = NULL;
  BXR_NEW(font);
  if (!font) {
    return NULL;
  }

  font->image        = bxr_image_create_mem(width, height, (void *)data);
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
  BXR_ASSERT(font);
  return font->image;
}

void
bxr_font_set_char_spacing(bxr_font_t *font, int spacing)
{
  BXR_ASSERT(font);
  font->char_spacing = spacing;
}

bxr_vec2_t
bxr_font_get_icon_range(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->icon_range;
}

bxr_vec2_t
bxr_font_get_char_range(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->char_range;
}

int
bxr_font_get_glyph_count(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->glyph_count;
}

bxr_rect_t
bxr_font_get_glyph_rect(const bxr_font_t *font, int glyphs_index)
{
  BXR_ASSERT(font);

  BXR_ASSERT(glyphs_index >= 0 && (size_t)glyphs_index < font->glyph_count);
  return font->glyphs[glyphs_index];
}

void
bxr_font_destroy(bxr_font_t *font)
{
  if (!font) {
    return;
  }

  bxr_image_destroy(font->image);
  BXR_FREE(font);
}

int
bxr_font_get_base(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->base;
}

int
bxr_font_get_char_spacing(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->char_spacing;
}

int
bxr_font_get_line_spacing(const bxr_font_t *font)
{
  BXR_ASSERT(font);
  return font->line_spacing;
}

void
bxr_font_set_line_spacing(bxr_font_t *font, int line_spacing)
{
  BXR_ASSERT(font);
  font->line_spacing = line_spacing;
}

bxr_vec2_t
bxr_font_measure_text(const bxr_font_t *font, const char *str)
{
  BXR_ASSERT(font);

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
        BXR_LOG_WARN(
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
