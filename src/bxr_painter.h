/**
 * @file bxr_painter.h
 *
 * @note The design of this painter is based on Sokol_gp (sokol graphics
 * painter) so I would like to thanks Edubart. Without his work, I would have
 * never been able to implement this painter in such a short time and as good as
 * it is.
 *
 * @brief Painter system for 2D rendering.
 *
 * Configurations are centralized in `bxr_config.h`.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_PAINTER_H_
#define BXR_PAINTER_H_

#include <SDL3/SDL_gpu.h>

#include "bxr_context.h"
#include "bxr_image.h"
#include "bxr_pipeline.h"
#include "bxr_text.h"

// Internal API
// ---------------------------------------------------

void bxr_painter_setup(bxr_context_t *context);
void bxr_painter_shutdown();

// Public API
// ---------------------------------------------------

void bxr_painter_begin(void);
void bxr_painter_flush(void);
void bxr_painter_end(void);

bxr_vec2_t bxr_painter_get_frame_dimension(void);

void
bxr_painter_set_projection(float left, float right, float bottom, float top);
void bxr_painter_reset_projection(void);
void bxr_painter_push_transform(void);
void bxr_painter_pop_transform(void);
void bxr_painter_reset_transform(void);
void bxr_painter_translate(float x, float y);
void bxr_painter_rotate_at(float angle, float ax, float ay);
void bxr_painter_scale(float sx, float sy);
void bxr_painter_scale_at(float sx, float sy, float ax, float ay);

void bxr_painter_set_pipeline(bxr_pipeline_t pipeline);
void bxr_painter_reset_pipeline(void);
void bxr_painter_set_uniform(const void *vs_data,
                             size_t vs_size,
                             const void *fs_data,
                             size_t fs_size);
void bxr_painter_reset_uniform(void);
void bxr_painter_set_blend_mode(bxr_blendmode_e blend_mode);
void bxr_painter_reset_blend_mode(void);
void bxr_painter_set_color(bxr_color_t color);
bxr_color_t bxr_painter_get_color(void);
void bxr_painter_reset_color(void);
void bxr_painter_set_image(int channel, bxr_image_t image);
bxr_image_t bxr_painter_get_image(int channel);
void bxr_painter_reset_image(int channel);
void bxr_painter_unset_image(int channel);
void bxr_painter_set_sampler(int channel, bxr_sampler_e sampler);
void bxr_painter_reset_sampler(int channel);

/**
 * @brief Set the viewport for subsequent draw calls. (Viewport is in screen
 * pixels).
 */
void bxr_painter_viewport(float x, float y, float w, float h);
void bxr_painter_reset_viewport(void);

/**
 * @brief Set the scissor for subsequent draw calls. Scissor is in screen pixels
 * relative to viewport). To reset scissor, set w and h to negative values.
 */
void bxr_painter_scissor(float x, float y, float w, float h);
void bxr_painter_reset_scissor(void);

void bxr_painter_clear(void);

void bxr_painter_draw(bxr_primitive_e primitive_type,
                      const bxr_vertex_t *vertices,
                      Uint32 vertices_count);

void bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count);
void bxr_painter_draw_point(bxr_vec2_t point);

void bxr_painter_draw_line(bxr_line_t line);
void bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count);
void bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count);

void bxr_painter_draw_triangle_filled(bxr_triangle_t triangle);
void bxr_painter_draw_triangles(const bxr_triangle_t *triangles, Uint32 count);
void bxr_painter_draw_triangles_strip(const bxr_vec2_t *points, Uint32 count);

void bxr_painter_draw_rect_filled(bxr_rect_t rect);
void bxr_painter_draw_rects_filled(const bxr_rect_t *rects, Uint32 count);

void bxr_painter_draw_rect_textured(int channel, bxr_textured_rect_t rect);
void bxr_painter_draw_rects_textured(int channel,
                                     const bxr_textured_rect_t *rects,
                                     Uint32 count);

#endif // BXR_PAINTER_H_
