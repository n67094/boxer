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

/*
- This code is kept here for reference and will be use in the future trough a
- tool static bxr_font_t * bxr_font_generate_atlas_from_ttf(TTF_Font
*ttf_font, bxr_vec2_t icon_range, bxr_vec2_t char_range, int char_spacing,
                                    int line_spacing)
{
  char *glyphs        = NULL;
  size_t glyphs_count = (icon_range.y - icon_range.x + 1)
                        + (char_range.y - char_range.x + 1)
                        + 1; // +1 for null terminator
  BXR_ALLOC(glyphs, glyphs_count);
  if (!glyphs) {
    // TODO handle error
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for font glyphs.");
    return NULL;
  }

  // Generate a string containing all icons and char.
  for (int i = icon_range.x; i <= icon_range.y; ++i) {
    *glyphs++ = (char)i;
  }
  for (int i = char_range.x; i <= char_range.y; ++i) {
    *glyphs++ = (char)i;
  }

  glyphs[glyphs_count - 1] = '\0'; // Null terminator

  // Render the glyphs to a surface
  SDL_Surface *surface = TTF_RenderText_Blended(
      ttf_font, glyphs, glyphs_count, (SDL_Color){ 255, 255, 255, 255 });

  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to render font glyphs to surface.");
    // TODO handle error
    BXR_FREE(ttf_font);
    BXR_FREE(glyphs);
    return NULL;
  }

  bxr_rect_t *glyph_rects = NULL;
  BXR_ALLOC(glyph_rects, glyphs_count * sizeof(bxr_rect_t));
  if (!glyph_rects) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for font glyph rects.");
    // TODO handle error
    BXR_FREE(ttf_font);
    BXR_FREE(glyphs);
    SDL_DestroySurface(surface);
    return NULL;
  }

  for (size_t i = 0; i < glyphs_count; ++i) {
    int min_x, max_x, min_y, max_y, advance;

    if (!TTF_GetGlyphMetrics(
            ttf_font, glyphs[i], &min_x, &max_x, &min_y, &max_y, &advance)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Failed to get glyph metrics for character '%c'.",
                   glyphs[i]);
      // TODO handle error
      BXR_FREE(ttf_font);
      BXR_FREE(glyphs);
      BXR_FREE(glyph_rects);
      SDL_DestroySurface(surface);
      return NULL;
    }

    glyph_rects[i] = (bxr_rect_t){
      .x = min_x,
      .y = min_y,
      .w = max_x - min_x,
      .h = max_y - min_y,
    };
  }

  bxr_font_t *font = NULL;
  BXR_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for TTF font.");
    // TODO handle error
    return NULL;
  }

  font->type = BXR_FONT_TYPE_TTF;

  font->ttf.image
      = bxr_image_load_mem(surface->w, surface->h, surface->pixels);
  SDL_DestroySurface(surface);
  font->ttf.glyphs      = glyph_rects;
  font->ttf.glyph_count = glyphs_count;
  font->icon_range      = icon_range;
  font->char_range      = char_range;
  font->line_spacing    = line_spacing;
  font->char_spacing    = char_spacing;
  font->base_char       = ' ';

  return font;
}

bxr_font_t *
bxr_font_load_ttf(const char *path,
                     bxr_vec2_t icon_range,
                     bxr_vec2_t char_range,
                     int font_size,
                     int char_spacing,
                     int line_spacing)
{
  SDL_assert(path);

  TTF_Font *ttf_font = TTF_OpenFont(path, font_size);
  if (!ttf_font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to load TTF font from path: %s",
                 path);
    // TODO handle error
    return NULL;
  }

  bxr_font_t *font = bxr_font_generate_atlas_from_ttf(
      ttf_font, icon_range, char_range, char_spacing, line_spacing);

  TTF_CloseFont(ttf_font);

  return font;
}

bxr_font_t *
bxr_font_load_ttf_mem(const void *data,
                         size_t data_size,
                         bxr_vec2_t icon_range,
                         bxr_vec2_t char_range,
                         int font_size,
                         int char_spacing,
                         int line_spacing)
{
  SDL_assert(data);

  TTF_Font *ttf_font
      = TTF_OpenFontIO(SDL_IOFromConstMem(data, data_size), true, font_size);

  bxr_font_t *font = bxr_font_generate_atlas_from_ttf(
      ttf_font, icon_range, char_range, char_spacing, line_spacing);

  TTF_CloseFont(ttf_font);

  return font;
}*/

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
