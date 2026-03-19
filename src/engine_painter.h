/**
 * @file engine_painter.h
 *
 * @note The design of this painter is based on Sokol_gp (sokol graphics
 * painter) so I would like to thanks Edubart. Without his work, I would have
 * never been able to implement this painter in such a short time and as good as
 * it is.
 *
 * @brief Painter system for 2D rendering.
 *
 * Configurations are centralized in `engine_config.h`.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_PAINTER_H_
#define ENGINE_PAINTER_H_

#include <SDL3/SDL_gpu.h>

#include "engine_context.h"
#include "engine_image.h"
#include "engine_pipeline.h"
#include "engine_text.h"

// Internal API
// ---------------------------------------------------

void engine_painter_setup(engine_context_t *context);
void engine_painter_shutdown();

// Public API
// ---------------------------------------------------

void engine_painter_begin(void);
void engine_painter_flush(void);
void engine_painter_end(void);

engine_vec2_t engine_painter_get_frame_size(void);

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

void engine_painter_set_pipeline(engine_pipeline_t pipeline);
void engine_painter_reset_pipeline(void);
void engine_painter_set_uniform(const void *vs_data,
                                size_t vs_size,
                                const void *fs_data,
                                size_t fs_size);
void engine_painter_reset_uniform(void);
void engine_painter_set_blend_mode(engine_blendmode_e blend_mode);
void engine_painter_reset_blend_mode(void);
void engine_painter_set_color(engine_color_t color);
engine_color_t engine_painter_get_color(void);
void engine_painter_reset_color(void);
void engine_painter_set_image(int channel, engine_image_t image);
engine_image_t engine_painter_get_image(int channel);
void engine_painter_reset_image(int channel);
void engine_painter_unset_image(int channel);
void engine_painter_set_sampler(int channel, engine_sampler_e sampler);
void engine_painter_reset_sampler(int channel);

/**
 * @brief Set the viewport for subsequent draw calls. (Viewport is in screen
 * pixels).
 */
void engine_painter_viewport(float x, float y, float w, float h);
void engine_painter_reset_viewport(void);

/**
 * @brief Set the scissor for subsequent draw calls. Scissor is in screen pixels
 * relative to viewport). To reset scissor, set w and h to negative values.
 */
void engine_painter_scissor(float x, float y, float w, float h);
void engine_painter_reset_scissor(void);

void engine_painter_clear(void);

void engine_painter_draw(engine_primitive_e primitive_type,
                         const engine_vertex_t *vertices,
                         Uint32 vertices_count);

void engine_painter_draw_points(const engine_vec2_t *points, Uint32 count);
void engine_painter_draw_point(engine_vec2_t point);

void engine_painter_draw_line(engine_line_t line);
void engine_painter_draw_lines(const engine_line_t *lines, Uint32 count);
void engine_painter_draw_lines_strip(const engine_vec2_t *points, Uint32 count);

void engine_painter_draw_triangle_filled(engine_triangle_t triangle);
void engine_painter_draw_triangles(const engine_triangle_t *triangles,
                                   Uint32 count);
void engine_painter_draw_triangles_strip(const engine_vec2_t *points,
                                         Uint32 count);

void engine_painter_draw_rect_filled(engine_rect_t rect);
void engine_painter_draw_rects_filled(const engine_rect_t *rects, Uint32 count);

void engine_painter_draw_rect_textured(int channel,
                                       engine_textured_rect_t rect);
void engine_painter_draw_rects_textured(int channel,
                                        const engine_textured_rect_t *rects,
                                        Uint32 count);

void engine_painter_draw_text(engine_text_t *text);

#endif // ENGINE_PAINTER_H_
