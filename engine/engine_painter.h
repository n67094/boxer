/**
 * @file engine_painter.h
 *
 * Thanks to edubart (Sokol_gp) for the painter inspiration.
 *
 * @brief Painter system for 2D rendering.
 *
 * @note Configuration are in `engine_config.h`.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_PAINTER_H_
#define ENGINE_PAINTER_H_

#include <SDL3/SDL_gpu.h>

#include "engine_context.h"
#include "engine_image.h"
#include "engine_pipeline.h"

typedef struct engine_vec2_s engine_point_t;

#define engine_point_make(x, y) ((const engine_point_t){ x, y })

typedef struct engine_line_s
{
  engine_point_t a;
  engine_point_t b;
} engine_line_t;

#define engine_line_make(ax, ay, bx, by)                                       \
  ((const engine_line_t){ { ax, ay }, { bx, by } })

typedef struct engine_triangle_s
{
  engine_vec2_t a;
  engine_vec2_t b;
  engine_vec2_t c;
} engine_triangle_t;

#define engine_triangle_make(ax, ay, bx, by, cx, cy)                           \
  ((const engine_triangle_t){ { ax, ay }, { bx, by }, { cx, cy } })

typedef struct engine_textured_rect_s
{
  engine_rect_t dst;
  engine_rect_t src;
} engine_textured_rect_t;

// Internal API
// ---------------------------------------------------

void engine_painter_setup(engine_context_t *context);
void engine_painter_shutdown();

// Public API
// ---------------------------------------------------

void engine_painter_begin(int width, int height);
void engine_painter_flush(void);
void engine_painter_end(void);

engine_vec2_t engine_painter_dimensions(void);

void
engine_painter_set_projection(float left, float right, float bottom, float top);
void engine_painter_reset_projection(void);
void engine_painter_push_transform(void);
void engine_painter_pop_transform(void);
void engine_painter_reset_transform(void);
void engine_painter_translate(float x, float y);
void engine_painter_rotate_at(float angle, float ax, float ay);
void engine_painter_scale(float sx, float sy);
void engine_painter_scale_at(float sx, float sy, float ax, float ay);

void engine_painter_set_blend_mode(engine_blendmode_e blend_mode);
void engine_painter_reset_blend_mode(void);
void engine_painter_set_color(engine_color_t color);
void engine_painter_reset_color(void);
void engine_painter_set_image(engine_image_t image);
void engine_painter_reset_image(void);
void engine_painter_set_sampler(engine_sampler_e sampler);
void engine_painter_reset_sampler(void);
void engine_painter_viewport(int x, int y, int w, int h);
void engine_painter_reset_viewport(void);
void engine_painter_scissor(int x, int y, int w, int h);
void engine_painter_reset_scissor(void);

void engine_painter_clear(void);

void engine_painter_draw(engine_primitive_e primitive_type,
                         const engine_vertex_t *vertices,
                         Uint32 vertices_count);

void engine_painter_draw_points(const engine_point_t *points, Uint32 count);
void engine_painter_draw_point(float x, float y);

void engine_painter_draw_lines(const engine_line_t *lines, Uint32 count);
void engine_painter_draw_line(float ax, float ay, float bx, float by);
void engine_painter_draw_lines_strip(const engine_point_t *points,
                                     Uint32 count);

void engine_painter_draw_triangles(const engine_triangle_t *triangles,
                                   Uint32 count);
void engine_painter_draw_filled_triangle(
    float ax, float ay, float bx, float by, float cx, float cy);
void engine_painter_draw_triangles_strip(const engine_point_t *points,
                                         Uint32 count);

void engine_painter_draw_filled_rects(const engine_rect_t *rects,
                                      uint32_t count);
void engine_painter_draw_filled_rect(engine_rect_t rect);

void engine_painter_draw_textured_rects(const engine_textured_rect_t *rects,
                                        Uint32 count);
void engine_painter_draw_textured_rect(engine_rect_t dest, engine_rect_t src);

#endif // ENGINE_PAINTER_H_
