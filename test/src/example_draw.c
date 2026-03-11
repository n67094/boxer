#include "engine.h"

#include "example_draw.h"

static engine_image_t _image;

void
example_draw_setup(void)
{
  _image = engine_image_load("data/images/logo.png");
}

void
example_draw_update(Uint64 delta_time_ms)
{
}

void
example_draw_render(Uint64 alpha_time_ms)
{
  engine_vec2_t frame_size = engine_painter_get_frame_size();

  // Seconds since start
  float time = (float)SDL_GetTicks() / 1000.0f;

  // Oscillate between -1 and 1 every second
  float osc_1 = engine_sin(time * ENGINE_PI);

  // Math for quadrants
  int hw = frame_size.x * 0.5f;
  int hh = frame_size.y * 0.5f;

  // Quadrant 1
  // ===============================================================
  //
  // Draw points in a grid pattern and a rectangle that scales based
  // on a sine wave.
  {
    engine_painter_viewport(0, 0, hw, hh);
    engine_painter_set_color(engine_color_make(50, 20, 20, 255));
    engine_painter_clear();

    float width  = hw;
    float height = hh;

    engine_vec2_t center = engine_vec2_make(width * 0.5f, height * 0.5f);

    int rect_w = width * 0.8f;
    int rect_h = height * 0.8f;

    int rect_hw = rect_w * 0.5f;
    int rect_hh = rect_h * 0.5f;

    engine_painter_push_transform();
    {
      // Draw a red rectangle
      engine_painter_set_color(engine_color_make(255, 0, 0, 255));
      {
        engine_vec2_t points[5];

        points[0] = engine_vec2_make(center.x - rect_hw, center.y - rect_hh);
        points[1] = engine_vec2_make(center.x + rect_hw, center.y - rect_hh);
        points[2] = engine_vec2_make(center.x + rect_hw, center.y + rect_hh);
        points[3] = engine_vec2_make(center.x - rect_hw, center.y + rect_hh);
        points[4] = points[0]; // Close the rectangle

        engine_painter_draw_lines_strip(points, 5);
      }

      // Draw white points
      engine_painter_set_color(engine_color_make(255, 255, 255, 255));
      {
        int step_w = width * 0.05f;  // 5% of the quadrant width
        int step_h = height * 0.05f; // 5% of the quadrant height

        for (int y = center.y - rect_hh + step_h; y < center.y + rect_hh;
             y += step_h) {
          for (int x = center.x - rect_hw + step_w; x < center.x + rect_hw;
               x += step_w) {
            engine_painter_draw_point(engine_vec2_make(x, y));
          }
        }
      }
    }
    engine_painter_pop_transform();
  }

  // Quadrant 2
  // ===============================================================
  // Draw a triangle that scales based on a sine wave and a rectangle that
  // translates based on a different sine wave.
  {
    engine_painter_viewport(hw, 0, hw, hh);
    engine_painter_set_color(engine_color_make(10, 10, 10, 255));
    engine_painter_clear();

    float width  = hw;
    float height = hh;

    float half_shape = width * 0.10f; // 10% of the quadrant width

    engine_painter_push_transform();
    {
      // Move the origin to the center of the left sub-quadrant
      engine_painter_translate(width * 0.25f, height * 0.5f);

      // Draw a magenta triangle that scales based on a sine wave
      engine_painter_set_color(engine_color_make(255, 0, 255, 255));
      engine_painter_push_transform();
      {
        // Scale between 0.5 and 1.5 based on osc_1
        engine_painter_scale(1.0f + osc_1 * 0.5f, 1.0f + osc_1 * 0.5f);

        engine_triangle_t triangle = engine_triangle_make(
            -half_shape, -half_shape, half_shape, -half_shape, 0, half_shape);

        engine_painter_draw_triangle_filled(triangle);
      }
      engine_painter_pop_transform();

      // Move the origin to the center of the right sub-quadrant
      engine_painter_translate(width * 0.5f, 0.0f);

      // Draw a cyan rectangle that translates based on a sine wave
      engine_painter_set_color(engine_color_make(0, 255, 255, 255));
      engine_painter_push_transform();
      {
        engine_painter_translate(osc_1 * width * 0.05f, 0.0f);

        engine_rect_t rect = engine_rect_make(
            -half_shape, -half_shape, half_shape * 2, half_shape * 2);

        engine_painter_draw_rect_filled(rect);
      }
      engine_painter_pop_transform();
    }
    engine_painter_pop_transform();
  }

  // Quadrant 3
  // =============================================================
  // Draw a rectangle with a different color at each corner and a
  // color wheel. (Thanks to edubart for the color wheel snippet).
  {
    engine_painter_viewport(0, hh, hw, hh);
    engine_painter_set_color(engine_color_make(10, 10, 10, 255));
    engine_painter_clear();

    float width  = hw;
    float height = hh;

    float half_shape = width * 0.10f; // 10% of the quadrant width

    engine_painter_push_transform();
    {
      // Move the origin to the center of the left sub-quadrant
      engine_painter_translate(width * 0.25f, height * 0.5f);

      // Draw a rectangle with a different color at each corner
      {
        static engine_color_t colors[4] = {
          engine_color_make(255, 100, 100, 255), // Top-left: Soft red
          engine_color_make(255, 180, 100, 255), // Top-right: Orange
          engine_color_make(180, 100, 255, 255), // Bottom-left: Purple
          engine_color_make(100, 180, 255, 255)  // Bottom-right: Sky blue
        };

        engine_vec2_t positions[4] = {
          engine_vec2_make(-half_shape, -half_shape), // Top-left
          engine_vec2_make(half_shape, -half_shape),  // Top-right
          engine_vec2_make(-half_shape, half_shape),  // Bottom-right
          engine_vec2_make(half_shape, half_shape)    // Bottom-left
        };

        static engine_vertex_t vertex_buffer[4];

        for (int i = 0; i < 4; ++i) {
          vertex_buffer[i].position = positions[i];
          vertex_buffer[i].color    = colors[i];
        }

        engine_painter_draw(ENGINE_PRIMITIVE_TRIANGLE_STRIP, vertex_buffer, 4);
      }

      // Move the origin to the center of the left sub-quadrant
      engine_painter_translate(width * 0.5f, 0.0f);

      // Draw a color wheel
      {
        float step         = (2.0f * ENGINE_PI2) / 64.0f;
        unsigned int count = 0;
        static engine_vertex_t vertex_buffer[4096];

        for (float theta = 0.0f; theta <= 2.0f * ENGINE_PI + step * 0.5f;
             theta += step) {

          engine_vec2_t v = engine_vec2_make(half_shape * engine_cos(theta),
                                             half_shape * engine_sin(theta));

          vertex_buffer[count].position = v;

          vertex_buffer[count].color = (engine_color_t){
            (engine_sin(theta + time * 1) + 1.0f) * 0.5f * 255.0f,
            (engine_sin(theta + time * 2) + 1.0f) * 0.5f * 255.0f,
            (engine_sin(theta + time * 4) + 1.0f) * 0.5f * 255.0f,
            255
          };
          count++;

          if (count % 3 == 1) {
            engine_vec2_t u               = engine_vec2_make(0, 0);
            vertex_buffer[count].position = u;
            vertex_buffer[count].color = (engine_color_t){ 255, 255, 255, 255 };
            count++;
          }
        }
        engine_painter_draw(
            ENGINE_PRIMITIVE_TRIANGLE_STRIP, vertex_buffer, count);
      }
      engine_painter_pop_transform();
    }

    // Quadrant 4
    // ============================================================
    // Draw a rotating textured rectangle scale times 2.
    {
      engine_painter_viewport(hw, hh, hw, hh);
      engine_painter_set_color(engine_color_make(20, 20, 20, 255));
      engine_painter_clear();

      float width  = hw;
      float height = hh;

      engine_painter_push_transform();
      {
        // Move the origin to the center of the quadrant
        engine_painter_translate(width * 0.5f, height * 0.5f);

        // Scale the entire quadrant by 2
        engine_painter_scale_at(2.0f, 2.0f, 0.0f, 0.0f);

        // Rotate the rectangle based on time
        engine_painter_rotate_at(time, 0.0f, 0.0f);

        engine_painter_set_color(engine_color_make(255, 255, 255, 255));

        int img_w = engine_image_get_width(_image);
        int img_h = engine_image_get_height(_image);

        engine_rect_t img_src
            = engine_rect_make(0, 0, (float)img_w, (float)img_h);

        engine_rect_t img_dst = engine_rect_make(
            -img_w * 0.5f, -img_h * 0.5f, (float)img_w, (float)img_h);

        engine_painter_set_image(_image);

        engine_painter_draw_rect_textured(
            engine_textured_rect_make(img_dst, img_src));
      }
      engine_painter_pop_transform();
    }
  }
}

void
example_draw_shutdown(void)
{
  engine_image_destroy(_image);
}
