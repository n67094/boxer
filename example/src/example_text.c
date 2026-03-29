/*
#include "bxr.h"

#include "example_text.h"

const size_t _font_glyph_count     = 4 + 100; // 4 icons + 96 chars
const bxr_rect_t _font_glyphs[] = {
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

static bxr_font_t *_font = NULL;
static bxr_text_t *_text = NULL;

static const char *_str_1 = "{b=00FF00FF}{f=FF0000FF}Hello, World!{/b}{/f}\n"
                            "This is an icon: {i=02}\n"
                            "\n1234567890\n!@#$%^&*()_+-=[]{}|;':\",./<>?";

static const char *_str_3
    = "This is a long text to use for {b=FF00FFFF}wrapping{/b} testing. It "
      "should wrap correctly and maintain the colors and icons. {i=01}.";

void
example_text_setup(void)
{
  _font = bxr_font_load("data/images/font.png",
                           _font_glyphs,
                           _font_glyph_count,
                           (bxr_vec2_t){ 0, 3 },
                           (bxr_vec2_t){ 4, 100 },
                           ' ',
                           0,
                           8);

  _text = bxr_text_make(
      _font, _str_1, ENGIEN_COLOR_MAGENTA, BXR_COLOR_WHITE);
}

void
example_text_update(Uint64 delta_time_ms)
{
}

void
example_text_render(Uint64 alpha_time_ms)
{
  bxr_vec2_t frame_size = bxr_painter_get_frame_size();

  float scale = 2.0f;
  int height  = frame_size.y / scale;

  // TODO text should position itself but the user have to position it when
  // drawing. text should only compute the quads and colors to render the font
  // and some wrapping as well as alignment but the position should be set when
  // drawing. This way we can reuse the same text with different positions and
  // transformations.

  bxr_painter_set_color(bxr_color_make(0, 0, 0, 255));
  bxr_painter_clear();

  bxr_painter_push_transform();
  {
    bxr_painter_scale(scale, scale);
    bxr_painter_translate(10.0f, 10.0f);

    // Uncached (immediate) text print example
    {
      bxr_text_t *immediate_text
          = bxr_text_make(_font,
                             "{f=000000FF}Immediate{/f} text example.",
                             BXR_COLOR_CYAN,
                             BXR_COLOR_WHITE);

      bxr_painter_set_blend_mode(BXR_BLENDMODE_BLEND);

      bxr_text_entry_t *entries
          = (bxr_text_entry_t *)bxr_text_get_entries(immediate_text);
      size_t entries_count = bxr_text_get_entries_count(immediate_text);

      for (size_t i = 0; i < entries_count; ++i) {
        bxr_painter_reset_image(0);

        bxr_painter_set_color(entries[i].background);
        bxr_painter_draw_rect_filled(entries[i].rects.dst);

        bxr_painter_set_image(0, bxr_font_get_image(_font));

        bxr_painter_set_color(entries[i].foreground);
        bxr_painter_draw_rect_textured(
            0,
            bxr_textured_rect_make(entries[i].rects.dst,
                                      entries[i].rects.src));
      }

      bxr_text_destroy(immediate_text);
    }

    bxr_painter_translate(0.0f, height * 0.25f);

    // Custom and cached text print example
    {
      bxr_painter_set_blend_mode(BXR_BLENDMODE_BLEND);

      bxr_text_entry_t *entries
          = (bxr_text_entry_t *)bxr_text_get_entries(_text);
      size_t entries_count = bxr_text_get_entries_count(_text);

      for (size_t i = 0; i < entries_count; ++i) {
        SDL_Log("Background r: %d g: %d b: %d a: %d",
                entries[i].background.r,
                entries[i].background.g,
                entries[i].background.b,
                entries[i].background.a);

        bxr_painter_reset_image(0);

        bxr_painter_set_color(entries[i].background);
        bxr_painter_draw_rect_filled(entries[i].rects.dst);

        bxr_painter_set_image(0, bxr_font_get_image(_font));

        bxr_painter_set_color(entries[i].foreground);
        bxr_painter_draw_rect_textured(
            0,
            bxr_textured_rect_make(entries[i].rects.dst,
                                      entries[i].rects.src));
      }
    }

    bxr_painter_translate(0.0f, height * 0.25f);

    // Cached text wrapping example
    {
      char *wrapped_str = bxr_text_wrap(_font, _str_3, 200.0f, NULL);

      bxr_text_t *wrapped_text = bxr_text_make(
          _font, wrapped_str, BXR_COLOR_CYAN, BXR_COLOR_YELLOW);

      bxr_painter_set_blend_mode(BXR_BLENDMODE_BLEND);

      bxr_text_entry_t *entries
          = (bxr_text_entry_t *)bxr_text_get_entries(wrapped_text);
      size_t entries_count = bxr_text_get_entries_count(wrapped_text);

      for (size_t i = 0; i < entries_count; ++i) {
        bxr_painter_reset_image(0);

        bxr_painter_set_color(entries[i].background);
        bxr_painter_draw_rect_filled(entries[i].rects.dst);

        bxr_painter_set_image(0, bxr_font_get_image(_font));

        bxr_painter_set_color(entries[i].foreground);
        bxr_painter_draw_rect_textured(
            0,
            bxr_textured_rect_make(entries[i].rects.dst,
                                      entries[i].rects.src));

        bxr_text_destroy(wrapped_text);
        BXR_FREE(wrapped_str);
      }

      bxr_text_destroy(wrapped_text);
    }
  }
  bxr_painter_pop_transform();

  bxr_painter_tanslate(1.0f, 1.0f);

  static const char *first_str = "Hello, World!\nIcon:[i=0]";

  bxr_rect_t first_str_box = bxr_text_measure(_font, first_str);
  bxr_painter_draw_rect_filled(first_str_box);

  bxr_painter_draw_text(first_str, _font, BXR_COLOR_MAGENTA);

  bxr_painter_tanslate(-1.0f, -1.0f);

  bxr_painter_draw_text(first_str, _font, BXR_COLOR_WHITE);

  static const char *second_str
      = "This is a [bgnd=FF0000FF][fgnd=00FF00FF]multi color text[end] and "
        "this is the an colorized icon: [fngd=<0000FFFF>][icon=1][end].";

  bxr_painter_draw_text_ex(second_str, _font, 200, BXR_TEXT_ALIGN_CENTER);
}

void
example_text_shutdown(void)
{
  bxr_text_destroy(_text);
}
*/
