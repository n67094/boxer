#include <engine.h>

static engine_image_t _image;

static engine_game_config_t _config = { 0 };

engine_game_config_t *
engine_game_config(void)
{
  _config.name  = "boxer_engine";
  _config.title = "Boxer Engine";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  _config.target_ups = 60;

  return &_config;
}

void
engine_game_setup(void)
{
  // Load an image
  _image = engine_image_load("data/images/logo.png");
}

void
engine_game_update(Uint64 delta_time_ms)
{
  // Update game logic here
}

void
engine_game_render(Uint64 alpha_time_ms)
{
  // Timing calculations
  // -------------------------------------------------------------------

  // Seconds since start
  float ticks = (float)SDL_GetTicks() / 1000.0f;
  // Oscillates between -1 and 1
  float osc_1 = engine_sin(ticks * ENGINE_PI2);
  // Oscillates enginetween 0.5 and 1.5 slowly
  float osc_2 = 1.0f + engine_sin(ticks / 2 * ENGINE_PI2) * 0.5f;

  engine_vec2_t window_dimensions = engine_context_get_window_dimensions();

  // Begin painter queue
  engine_painter_begin(window_dimensions.x / 2,
                       window_dimensions.y / 2); // zoom x2

  // Draw 4 pink points that form a square and rotate over time.
  {
    // Center
    int center_x = 1024 * 0.5f;
    int center_y = 768 * 0.5f;

    // Rectangle 50x50 points
    float size   = 50;          // Size of the square
    float h_size = size * 0.5f; // Half size

    engine_point_t points[4] = {
      { center_x - h_size, center_y - h_size }, // top-left
      { center_x + h_size, center_y - h_size }, // top-right
      { center_x - h_size, center_y + h_size }, // bottom-left
      { center_x + h_size, center_y + h_size }, // bottom-right
    };

    engine_painter_set_color(255, 0, 255, 255); // Pink color

    engine_painter_push_transform();

    // Scale around the center of the square
    engine_painter_scale_at(
        osc_2, osc_2, points[0].x + h_size, points[0].y + h_size);

    // Rotate around the center of the square
    engine_painter_rotate_at(ticks, points[0].x + h_size, points[0].y + h_size);

    // Draw the points
    engine_painter_draw_points(points, 4);

    engine_painter_pop_transform();
  }

  // Draw a pink triangle
  {
    engine_painter_set_color(255, 0, 255, 255);
    engine_painter_draw_filled_triangle(150, 60, 200, 110, 100, 110);
  }

  // Draw a line
  {
    engine_painter_push_transform();

    engine_painter_rotate_at(ticks * 20.0f, 70.0f, 180.0f);

    engine_painter_set_color(0, 255, 0, 255);
    engine_painter_draw_line(20, 180, 120, 180);

    engine_painter_pop_transform();
  }

  // Draw a cyan rectangle
  {
    engine_painter_push_transform();
    engine_painter_translate(osc_2 * 20.0f, 0.0f);

    engine_painter_set_color(0, 255, 255, 255);
    engine_painter_draw_filled_rect(50, 60, 50, 50);

    engine_painter_pop_transform();
  }

  // Draw custom triangles
  {
    engine_painter_set_color(255, 255, 255, 255);
    static engine_vertex_t vertex_buffer[3];

    vertex_buffer[0].position = engine_vec2_make(150, 200);
    vertex_buffer[0].color    = engine_color_make(255, 0, 0, 255);
    vertex_buffer[1].position = engine_vec2_make(100, 250);
    vertex_buffer[1].color    = engine_color_make(0, 255, 0, 255);
    vertex_buffer[2].position = engine_vec2_make(200, 250);
    vertex_buffer[2].color    = engine_color_make(0, 0, 255, 255);

    engine_painter_draw(ENGINE_PRIMITIVE_TRIANGLES, vertex_buffer, 3);
  }

  // Draw textured rectangle
  {
    int width  = engine_image_get_width(_image);
    int height = engine_image_get_height(_image);

    engine_rect_t img_src = engine_rect_make(0, 0, (float)width, (float)height);
    engine_rect_t img_dst
        = engine_rect_make(220, 80, (float)width, (float)height);

    engine_painter_set_image(_image);
    engine_painter_set_color(255, 255, 255, 255);

    engine_painter_push_transform();

    engine_painter_rotate_at(
        -ticks, img_dst.x + img_dst.w / 2.0f, img_dst.y + img_dst.h / 2.0f);

    engine_painter_draw_textured_rect(img_dst, img_src);

    engine_painter_pop_transform();
  }
  engine_painter_flush();
  engine_painter_end();
}

void
engine_game_shutdown(void)
{
  engine_image_destroy(_image);
}
