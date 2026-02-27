#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "engine_font.h"
#include "engine_image.h"
#include "engine_mem.h"

static engine_font_t *
engine_font_generate_atlas_from_ttf(TTF_Font *ttf_font,
                                    engine_vec2_t icon_range,
                                    engine_vec2_t char_range,
                                    int char_spacing,
                                    int line_spacing)
{
  char *glyphs = NULL;
  size_t glyphs_count
      = (icon_range.y - icon_range.x + 1) + (char_range.y - char_range.x + 1);
  ENGINE_ALLOC(glyphs, glyphs_count);
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

  // Render the glyphs to a surface
  SDL_Surface *surface = TTF_RenderText_Blended(
      ttf_font, glyphs, glyphs_count, (SDL_Color){ 255, 255, 255, 255 });

  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to render font glyphs to surface.");
    // TODO handle error
    ENGINE_FREE(ttf_font);
    ENGINE_FREE(glyphs);
    return NULL;
  }

  engine_rect_t *glyph_rects = NULL;
  ENGINE_ALLOC(glyph_rects, glyphs_count * sizeof(engine_rect_t));
  if (!glyph_rects) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for font glyph rects.");
    // TODO handle error
    ENGINE_FREE(ttf_font);
    ENGINE_FREE(glyphs);
    SDL_FreeSurface(surface);
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
      ENGINE_FREE(ttf_font);
      ENGINE_FREE(glyphs);
      ENGINE_FREE(glyph_rects);
      SDL_FreeSurface(surface);
      return NULL;
    }

    glyph_rects[i] = (engine_rect_t){
      .x = min_x,
      .y = min_y,
      .w = max_x - min_x,
      .h = max_y - min_y,
    };
  }

  engine_font_t *font = NULL;
  ENGINE_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for TTF font.");
    // TODO handle error
    return NULL;
  }

  font->image = engine_image_load_mem(surface->w, surface->h, surface->pixels);
  SDL_FreeSurface(surface);

  font->glyphs              = glyph_rects;
  font->glyph_count         = glyphs_count;
  font->free_glyphs         = true;
  font->glyph_spacing_index = char_range.x;
  font->char_spacing        = char_spacing;
  font->line_spacing        = line_spacing;
}

engine_font_t *
engine_font_load_ttf(const char *path,
                     engine_vec2_t icon_range,
                     engine_vec2_t char_range,
                     int font_size,
                     int char_spacing,
                     int line_spacing)
{
  SDL_assert(path);

  TTF_Font *ttf_font = TTF_OpenFont(path, font_size);

  engine_font_t *font = engine_font_generate_atlas_from_ttf(
      ttf_font, icon_range, char_range, char_spacing, line_spacing);

  TTF_CloseFont(ttf_font);

  return font;
}

engine_font_t *
engine_font_load_ttf_mem(const void *data,
                         size_t data_size,
                         engine_vec2_t icon_range,
                         engine_vec2_t char_range,
                         int font_size,
                         int char_spacing,
                         int line_spacing)
{
  SDL_assert(data);

  TTF_Font *ttf_font
      = TTF_OpenFontIO(SDL_IOFromConstMem(data, data_size), true, font_size);

  engine_font_t *font = engine_font_generate_atlas_from_ttf(
      ttf_font, icon_range, char_range, char_spacing, line_spacing);

  TTF_CloseFont(ttf_font);

  return font;
}

engine_font_t *
engine_font_atlas_load(const char *path,
                       engine_rect_t *glyphs,
                       size_t glyph_count,
                       int glyph_spacing_index,
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
                      int glyph_spacing_index,
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
  if (!font) {
    engine_image_destroy(font->image);

    if (font->free_glyphs) {
      ENGINE_FREE(font->glyphs);
    }

    ENGINE_FREE(font);
  }
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

engine_rect_t
engine_font_get_glyph_rect(const engine_font_t *font, int glyphs_index)
{
  SDL_assert(font);
  SDL_assert(glyphs_index >= 0 && (size_t)glyphs_index < font->glyph_count);

  return font->glyphs[glyphs_index];
}
