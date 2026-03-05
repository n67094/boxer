#include "engine.h"

#include "test_font.h"

const size_t _font_glyph_count     = 4 + 100; // 4 icons + 96 chars
const engine_rect_t _font_glyphs[] = {
  // Icons (4)
  { 0, 0, 8, 8 },
  { 8, 0, 8, 8 },
  { 16, 0, 8, 8 },
  { 24, 0, 8, 8 },
  // Chars (126) - row 0 continued
  { 32, 0, 8, 8 },
  { 40, 0, 8, 8 },
  { 48, 0, 8, 8 },
  { 56, 0, 8, 8 },
  { 64, 0, 8, 8 },
  { 72, 0, 8, 8 },
  { 80, 0, 8, 8 },
  { 88, 0, 8, 8 },
  { 96, 0, 8, 8 },
  { 104, 0, 8, 8 },
  { 112, 0, 8, 8 },
  { 120, 0, 8, 8 },
  // row 1
  { 0, 8, 8, 8 },
  { 8, 8, 8, 8 },
  { 16, 8, 8, 8 },
  { 24, 8, 8, 8 },
  { 32, 8, 8, 8 },
  { 40, 8, 8, 8 },
  { 48, 8, 8, 8 },
  { 56, 8, 8, 8 },
  { 64, 8, 8, 8 },
  { 72, 8, 8, 8 },
  { 80, 8, 8, 8 },
  { 88, 8, 8, 8 },
  { 96, 8, 8, 8 },
  { 104, 8, 8, 8 },
  { 112, 8, 8, 8 },
  { 120, 8, 8, 8 },
  // row 2
  { 0, 16, 8, 8 },
  { 8, 16, 8, 8 },
  { 16, 16, 8, 8 },
  { 24, 16, 8, 8 },
  { 32, 16, 8, 8 },
  { 40, 16, 8, 8 },
  { 48, 16, 8, 8 },
  { 56, 16, 8, 8 },
  { 64, 16, 8, 8 },
  { 72, 16, 8, 8 },
  { 80, 16, 8, 8 },
  { 88, 16, 8, 8 },
  { 96, 16, 8, 8 },
  { 104, 16, 8, 8 },
  { 112, 16, 8, 8 },
  { 120, 16, 8, 8 },
  // row 3
  { 0, 24, 8, 8 },
  { 8, 24, 8, 8 },
  { 16, 24, 8, 8 },
  { 24, 24, 8, 8 },
  { 32, 24, 8, 8 },
  { 40, 24, 8, 8 },
  { 48, 24, 8, 8 },
  { 56, 24, 8, 8 },
  { 64, 24, 8, 8 },
  { 72, 24, 8, 8 },
  { 80, 24, 8, 8 },
  { 88, 24, 8, 8 },
  { 96, 24, 8, 8 },
  { 104, 24, 8, 8 },
  { 112, 24, 8, 8 },
  { 120, 24, 8, 8 },
  // row 4
  { 0, 32, 8, 8 },
  { 8, 32, 8, 8 },
  { 16, 32, 8, 8 },
  { 24, 32, 8, 8 },
  { 32, 32, 8, 8 },
  { 40, 32, 8, 8 },
  { 48, 32, 8, 8 },
  { 56, 32, 8, 8 },
  { 64, 32, 8, 8 },
  { 72, 32, 8, 8 },
  { 80, 32, 8, 8 },
  { 88, 32, 8, 8 },
  { 96, 32, 8, 8 },
  { 104, 32, 8, 8 },
  { 112, 32, 8, 8 },
  { 120, 32, 8, 8 },
  // row 5
  { 0, 40, 8, 8 },
  { 8, 40, 8, 8 },
  { 16, 40, 8, 8 },
  { 24, 40, 8, 8 },
  { 32, 40, 8, 8 },
  { 40, 40, 8, 8 },
  { 48, 40, 8, 8 },
  { 56, 40, 8, 8 },
  { 64, 40, 8, 8 },
  { 72, 40, 8, 8 },
  { 80, 40, 8, 8 },
  { 88, 40, 8, 8 },
  { 96, 40, 8, 8 },
  { 104, 40, 8, 8 },
  { 112, 40, 8, 8 },
  { 120, 40, 8, 8 },
  // row 6
  { 0, 48, 8, 8 },
  { 8, 48, 8, 8 },
  { 16, 48, 8, 8 }
};

static engine_font_t *_font = NULL;
static engine_text_t *_text = NULL;

static const char *_str = "{b=00FF0000}{f=FF000000}Hello, World!{/b}{/f}\nThis "
                          "is a test of the font rendering\n"
                          "This is an icon: {i=02}\n"
                          "system.\n1234567890\n!@#$%^&*()_+-=[]{}|;':\",./<>?";

void
test_font_setup(void)
{
  _font = engine_font_load("data/images/font.png",
                           _font_glyphs,
                           _font_glyph_count,
                           (engine_vec2_t){ 0, 3 },
                           (engine_vec2_t){ 4, 100 },
                           ' ',
                           0,
                           8);

  // FIXME Engine text make shouldn't take a position as param
  _text = engine_text_make((engine_vec2_t){ 10, 10 },
                           _font,
                           _str,
                           ENGINE_COLOR_BLACK,
                           ENGINE_COLOR_WHITE);
}

void
test_font_update(Uint64 delta_time_ms)
{
}

void
test_font_render(Uint64 alpha_time_ms)
{
  engine_painter_set_color(engine_color_make(0, 0, 255, 255));
  engine_painter_clear();

  // FIXME a engine_painter_draw_text(position, text) would be better
  engine_painter_set_image(engine_font_get_image(_font));

  engine_text_entry_t *entries
      = (engine_text_entry_t *)engine_text_get_text_entries(_text);
  size_t entries_count = engine_text_get_text_entries_count(_text);

  for (size_t i = 0; i < entries_count; ++i) {
    engine_painter_set_color(entries[i].background);
    engine_painter_draw_filled_rect(entries[i].rects.dst);

    engine_painter_set_color(entries[i].foreground);
    engine_painter_draw_textured_rect(entries[i].rects.dst,
                                      entries[i].rects.src);
  }
}

void
test_font_shutdown(void)
{
  engine_text_destroy(_text);
}
