#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "engine_font.h"
#include "engine_image.h"
#include "engine_mem.h"

typedef enum
{
  ENGINE_FONT_TYPE_TTF,
  ENGINE_FONT_TYPE_ATLAS,
} engine_font_type_e;

typedef struct _engine_font_ttf_s
{
  TTF_Font *font;
} _engine_font_ttf_t;

typedef struct _engine_font_atlas_s
{
  engine_image_t image;
  engine_rect_t *glyphs;
  size_t glyph_count;
  float spacing;
  float line_spacing;
} _engine_font_atlas_t;

struct engine_font_s
{
  engine_font_type_e type;
  union
  {
    _engine_font_ttf_t ttf;
    _engine_font_atlas_t atlas;
  };
  float line_spacing;
};

engine_font_t *
engine_font_ttf_load(const char *path, unsigned int size)
{
  SDL_assert(path);

  engine_font_t *font = NULL;
  ENGINE_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for TTF font");
    return NULL;
  }

  font->type     = ENGINE_FONT_TYPE_TTF;
  font->ttf.font = TTF_OpenFont(path, (int)size);

  return font;
}

engine_font_t *
engine_font_ttf_mem(const void *data, size_t len, unsigned int size)
{
  SDL_assert(data);
  SDL_assert(len > 0);

  engine_font_t *font = NULL;
  ENGINE_NEW(font);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate memory for TTF font");
    return NULL;
  }

  font->type = ENGINE_FONT_TYPE_TTF;
  font->ttf.font
      = TTF_OpenFontIO(SDL_IOFromConstMem(data, (int)len), true, (int)size);

  return font;
}

engine_font_t *
engine_font_atlas_load(const char *path,
                       engine_rect_t *glyphs,
                       size_t glyph_count,
                       int spacing,
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

  font->type = ENGINE_FONT_TYPE_ATLAS;

  font->atlas.image        = engine_image_load(path);
  font->atlas.glyphs       = glyphs;
  font->atlas.glyph_count  = glyph_count;
  font->atlas.spacing      = spacing;
  font->atlas.line_spacing = line_spacing;

  return font;
}

engine_font_t *
engine_font_atlas_mem(unsigned int width,
                      unsigned int height,
                      const void *data,
                      engine_rect_t *glyphs,
                      size_t glyph_count,
                      int spacing,
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

  font->type = ENGINE_FONT_TYPE_ATLAS;

  font->atlas.image        = engine_image_load_mem(width, height, (void *)data);
  font->atlas.glyphs       = glyphs;
  font->atlas.glyph_count  = glyph_count;
  font->atlas.spacing      = spacing;
  font->atlas.line_spacing = line_spacing;

  return font;
}

void
engine_font_destroy(engine_font_t *font)
{
  if (!font)
    return;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    TTF_CloseFont(font->ttf.font);
    break;
  case ENGINE_FONT_TYPE_ATLAS:
    engine_image_destroy(font->atlas.image);
    break;
  default:
    break;
  }

  ENGINE_FREE(font);
}

int
engine_font_get_height(const engine_font_t *font)
{
  if (!font)
    return 0.0f;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    return TTF_GetFontHeight(font->ttf.font);
  case ENGINE_FONT_TYPE_ATLAS:
    // For atlas font, we can use the line spacing as the height of the font
    return font->line_spacing;
  default:
    return 0.0f;
  }
}

int
engine_font_get_spacing(const engine_font_t *font)
{
  if (!font)
    return 0.0f;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    // TODO avail in 3.4.0 return TTF_GetFontCharSpacing(font->ttf.font);
  case ENGINE_FONT_TYPE_ATLAS:
    return font->atlas.spacing;
  default:
    return 0.0f;
  }
}

void
engine_font_set_spacing(engine_font_t *font, int spacing)
{
  if (!font)
    return;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    // TODO avail in 3.4.0 TTF_SetFontCharSpacing(font->ttf.font, (int)spacing);
    break;
  case ENGINE_FONT_TYPE_ATLAS:
    font->atlas.spacing = spacing;
    break;
  default:
    break;
  }
}

int
engine_font_get_line_spacing(const engine_font_t *font)
{
  if (!font)
    return 0.0f;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    return TTF_GetFontLineSkip(font->ttf.font);
  case ENGINE_FONT_TYPE_ATLAS:
    return font->atlas.line_spacing;
  default:
    return 0.0f;
  }
}

void
engine_font_set_line_spacing(engine_font_t *font, int line_spacing)
{
  if (!font)
    return;

  switch (font->type) {
  case ENGINE_FONT_TYPE_TTF:
    TTF_SetFontLineSkip(font->ttf.font, (int)line_spacing);
    break;
  case ENGINE_FONT_TYPE_ATLAS:
    font->atlas.line_spacing = line_spacing;
    break;
  default:
    break;
  }
}
