#include "bxr.h"

#include "example_draw.h"

static bxr_image_t _image;

void
example_draw_setup(void)
{
  _image = bxr_image_load("data/images/logo.png");
}

void
example_draw_update(Uint64 delta_time_ms)
{
}

void
example_draw_render(Uint64 alpha_time_ms)
{
  bxr_vec2_t frame_dimension = bxr_painter_get_frame_dimension();

  // Seconds since start
  float time = (float)SDL_GetTicks() / 1000.0f;

  // Oscillate between -1 and 1 every second
  float osc_1 = bxr_sin(time * BXR_PI);

  // Math for quadrants
  int hw = frame_dimension.x * 0.5f;
  int hh = frame_dimension.y * 0.5f;

  // Quadrant 1
  // ===============================================================
  //
  // Draw points in a grid pattern and a rectangle that scales based
  // on a sine wave.
  {
    bxr_painter_viewport(0, 0, hw, hh);
    bxr_painter_set_color(bxr_color_make(50, 20, 20, 255));
    bxr_painter_clear();

    float width  = hw;
    float height = hh;

    bxr_vec2_t center = bxr_vec2_make(width * 0.5f, height * 0.5f);

    int rect_w = width * 0.8f;
    int rect_h = height * 0.8f;

    int rect_hw = rect_w * 0.5f;
    int rect_hh = rect_h * 0.5f;

    bxr_painter_push_transform();
    {
      // Draw a red rectangle
      bxr_painter_set_color(bxr_color_make(255, 0, 0, 255));
      {
        bxr_vec2_t points[5];

        points[0] = bxr_vec2_make(center.x - rect_hw, center.y - rect_hh);
        points[1] = bxr_vec2_make(center.x + rect_hw, center.y - rect_hh);
        points[2] = bxr_vec2_make(center.x + rect_hw, center.y + rect_hh);
        points[3] = bxr_vec2_make(center.x - rect_hw, center.y + rect_hh);
        points[4] = points[0]; // Close the rectangle

        bxr_painter_draw_lines_strip(points, 5);
      }

      // Draw white points
      bxr_painter_set_color(bxr_color_make(255, 255, 255, 255));
      {
        int step_w = width * 0.05f;  // 5% of the quadrant width
        int step_h = height * 0.05f; // 5% of the quadrant height

        for (int y = center.y - rect_hh + step_h; y < center.y + rect_hh;
             y += step_h) {
          for (int x = center.x - rect_hw + step_w; x < center.x + rect_hw;
               x += step_w) {
            bxr_painter_draw_point(bxr_vec2_make(x, y));
          }
        }
      }
    }
    bxr_painter_pop_transform();
  }

  // Quadrant 2
  // ===============================================================
  // Draw a triangle that scales based on a sine wave and a rectangle that
  // translates based on a different sine wave.
  {
    bxr_painter_viewport(hw, 0, hw, hh);
    bxr_painter_set_color(bxr_color_make(10, 10, 10, 255));
    bxr_painter_clear();

    float width  = hw;
    float height = hh;

    float half_shape = width * 0.10f; // 10% of the quadrant width

    bxr_painter_push_transform();
    {
      // Move the origin to the center of the left sub-quadrant
      bxr_painter_translate(width * 0.25f, height * 0.5f);

      // Draw a magenta triangle that scales based on a sine wave
      bxr_painter_set_color(bxr_color_make(255, 0, 255, 255));
      bxr_painter_push_transform();
      {
        // Scale between 0.5 and 1.5 based on osc_1
        bxr_painter_scale(1.0f + osc_1 * 0.5f, 1.0f + osc_1 * 0.5f);

        bxr_triangle_t triangle = bxr_triangle_make(
            -half_shape, -half_shape, half_shape, -half_shape, 0, half_shape);

        bxr_painter_draw_triangle_filled(triangle);
      }
      bxr_painter_pop_transform();

      // Move the origin to the center of the right sub-quadrant
      bxr_painter_translate(width * 0.5f, 0.0f);

      // Draw a cyan rectangle that translates based on a sine wave
      bxr_painter_set_color(bxr_color_make(0, 255, 255, 255));
      bxr_painter_push_transform();
      {
        bxr_painter_translate(osc_1 * width * 0.05f, 0.0f);

        bxr_rect_t rect = bxr_rect_make(
            -half_shape, -half_shape, half_shape * 2, half_shape * 2);

        bxr_painter_draw_rect_filled(rect);
      }
      bxr_painter_pop_transform();
    }
    bxr_painter_pop_transform();
  }

  // Quadrant 3
  // =============================================================
  // Draw a rectangle with a different color at each corner and a
  // color wheel. (Thanks to edubart for the color wheel snippet).
  {
    bxr_painter_viewport(0, hh, hw, hh);
    bxr_painter_set_color(bxr_color_make(10, 10, 10, 255));
    bxr_painter_clear();

    float width  = hw;
    float height = hh;

    float half_shape = width * 0.10f; // 10% of the quadrant width

    bxr_painter_push_transform();
    {
      // Move the origin to the center of the left sub-quadrant
      bxr_painter_translate(width * 0.25f, height * 0.5f);

      // Draw a rectangle with a different color at each corner
      {
        static bxr_color_t colors[4] = {
          bxr_color_make(255, 100, 100, 255), // Top-left: Soft red
          bxr_color_make(255, 180, 100, 255), // Top-right: Orange
          bxr_color_make(180, 100, 255, 255), // Bottom-left: Purple
          bxr_color_make(100, 180, 255, 255)  // Bottom-right: Sky blue
        };

        bxr_vec2_t positions[4] = {
          bxr_vec2_make(-half_shape, -half_shape), // Top-left
          bxr_vec2_make(half_shape, -half_shape),  // Top-right
          bxr_vec2_make(-half_shape, half_shape),  // Bottom-right
          bxr_vec2_make(half_shape, half_shape)    // Bottom-left
        };

        static bxr_vertex_t vertex_buffer[4];

        for (int i = 0; i < 4; ++i) {
          vertex_buffer[i].position = positions[i];
          vertex_buffer[i].color    = colors[i];
        }

        bxr_painter_draw(BXR_PRIMITIVE_TRIANGLE_STRIP, vertex_buffer, 4);
      }

      // Move the origin to the center of the left sub-quadrant
      bxr_painter_translate(width * 0.5f, 0.0f);

      // Draw a color wheel
      {
        float step         = (2.0f * BXR_PI2) / 64.0f;
        unsigned int count = 0;
        static bxr_vertex_t vertex_buffer[4096];

        for (float theta = 0.0f; theta <= 2.0f * BXR_PI + step * 0.5f;
             theta += step) {

          bxr_vec2_t v = bxr_vec2_make(half_shape * bxr_cos(theta),
                                       half_shape * bxr_sin(theta));

          vertex_buffer[count].position = v;

          vertex_buffer[count].color = (bxr_color_t){
            (bxr_sin(theta + time * 1) + 1.0f) * 0.5f * 255.0f,
            (bxr_sin(theta + time * 2) + 1.0f) * 0.5f * 255.0f,
            (bxr_sin(theta + time * 4) + 1.0f) * 0.5f * 255.0f,
            255
          };
          count++;

          if (count % 3 == 1) {
            bxr_vec2_t u                  = bxr_vec2_make(0, 0);
            vertex_buffer[count].position = u;
            vertex_buffer[count].color    = (bxr_color_t){ 255, 255, 255, 255 };
            count++;
          }
        }
        bxr_painter_draw(BXR_PRIMITIVE_TRIANGLE_STRIP, vertex_buffer, count);
      }
      bxr_painter_pop_transform();
    }

    // Quadrant 4
    // ============================================================
    // Draw a rotating textured rectangle scale times 2.
    {
      bxr_painter_viewport(hw, hh, hw, hh);
      bxr_painter_set_color(bxr_color_make(20, 20, 20, 255));
      bxr_painter_clear();

      float width  = hw;
      float height = hh;

      bxr_painter_push_transform();
      {
        // Move the origin to the center of the quadrant
        bxr_painter_translate(width * 0.5f, height * 0.5f);

        // Scale the entire quadrant by 2
        bxr_painter_scale_at(2.0f, 2.0f, 0.0f, 0.0f);

        // Rotate the rectangle based on time
        bxr_painter_rotate_at(time, 0.0f, 0.0f);

        bxr_painter_set_color(bxr_color_make(255, 255, 255, 255));

        int img_w = bxr_image_get_width(_image);
        int img_h = bxr_image_get_height(_image);

        bxr_rect_t img_src = bxr_rect_make(0, 0, (float)img_w, (float)img_h);

        bxr_rect_t img_dst = bxr_rect_make(
            -img_w * 0.5f, -img_h * 0.5f, (float)img_w, (float)img_h);

        bxr_painter_set_image(0, _image);

        bxr_painter_draw_rect_textured(
            0, bxr_textured_rect_make(img_dst, img_src));
      }
      bxr_painter_pop_transform();
    }
  }
}

void
example_draw_shutdown(void)
{
  bxr_image_destroy(_image);
}
