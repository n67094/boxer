/**
 * File `bxr_painter.h`.
 *
 * # Boxer's 2D Painter.
 *
 * Immediate-mode 2D renderer (thin wrapper over SDL_gp).
 *
 * Public API below may only be called from `bxr_game_update()` or
 * `bxr_game_render()`.
 *
 * Default coordinate space: virtual resolution in pixels, origin at the
 * top-left corner, y pointing down.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_PAINTER_H_
#define BXR_PAINTER_H_

#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_defs.h"
#include "bxr_image.h"
#include "bxr_math.h"
#include "bxr_pipeline.h"
#include "bxr_sampler.h"

typedef struct bxr_painter_desc_t
{
  Uint32 max_vertices;
  Uint32 max_commands;
  SDL_Window *window;
  SDL_GPUDevice *gpu_device;
} bxr_painter_desc_t;

typedef struct bxr_vertex_s
{
  bxr_vec2_t position;
  bxr_vec2_t texcoord;
  bxr_color_t color;
} bxr_vertex_t;

typedef struct bxr_textured_rect_s
{
  bxr_rect_t dst;
  bxr_rect_t src;
} bxr_textured_rect_t;

/**
 * Constructs a `bxr_textured_rect_t` compound literal from the `dst`
 * destination region (in the current coordinate space) and the `src` region
 * in the texture.
 */
#define bxr_textured_rect_create(dst, src)                                     \
  ((const bxr_textured_rect_t){ dst, src })

/**
 * ## Internal API
 */

/**
 * Setup the painter system.
 */
void bxr_painter_setup(const bxr_painter_desc_t *desc);

/**
 * Tears down the painter system and releases its GPU resources.
 */
void bxr_painter_teardown(void);

/**
 * ## Public API
 */

/**
 * Set the coordinate space boundaries mapped onto the current viewport.
 *
 * `left` and `right` the x coordinates of the left and right edges.
 * `bottom` and `top` the y coordinates of the bottom and top edges.
 */
BXR_API void
bxr_painter_set_projection(float left, float right, float bottom, float top);

/**
 * Reset the projection to the default coordinate space: viewport pixels,
 * origin at the top-left corner, y pointing down.
 */
BXR_API void bxr_painter_reset_projection(void);

/**
 * Save the current transform matrix on the transform stack.
 *
 * Every push must be matched by a `bxr_painter_pop_transform()` in the same
 * frame.
 */
BXR_API void bxr_painter_push_transform(void);

/**
 * Restore the transform matrix saved by the last
 * `bxr_painter_push_transform()`.
 */
BXR_API void bxr_painter_pop_transform(void);

/**
 * Reset the current transform matrix to identity.
 */
BXR_API void bxr_painter_reset_transform(void);

/**
 * Translate the coordinate space of subsequent draws.
 *
 * `x` and `y` the translation along each axis, in current units.
 */
BXR_API void bxr_painter_translate(float x, float y);

/**
 * Rotate the coordinate space of subsequent draws around the origin.
 *
 * `angle` the angle in radians. Positive is clockwise on screen with the
 * default y-down projection.
 */
BXR_API void bxr_painter_rotate(float angle);

/**
 * Rotate the coordinate space of subsequent draws around a point.
 *
 * `angle` the angle in radians. Positive is clockwise on screen with the
 * default y-down projection.
 * `ax` and `ay` the pivot, in current coordinates.
 */
BXR_API void bxr_painter_rotate_at(float angle, float ax, float ay);

/**
 * Scale the coordinate space of subsequent draws around the origin.
 *
 * `sx` and `sy` the scale factors along each axis. Negative values mirror.
 */
BXR_API void bxr_painter_scale(float sx, float sy);

/**
 * Scale the coordinate space of subsequent draws around a point.
 *
 * `sx` and `sy` the scale factors along each axis. Negative values mirror.
 * `ax` and `ay` the pivot, in current coordinates.
 */
BXR_API void bxr_painter_scale_at(float sx, float sy, float ax, float ay);

/**
 * Set the pipeline used by subsequent draws.
 *
 * `pipeline` a pipeline from `bxr_pipeline_create()`; ownership stays with
 * the caller, who must keep it alive until the frame is flushed.
 */
BXR_API void bxr_painter_set_pipeline(bxr_pipeline_t pipeline);

/**
 * Reset to the builtin pipeline. Also clears the current uniform data.
 */
BXR_API void bxr_painter_reset_pipeline(void);

/**
 * Set uniform data for subsequent draws with the current pipeline.
 *
 * The data is copied. `vs_size + fs_size` must not exceed
 * `BXR_PAINTER_MAX_UNIFORM_FLOATS * sizeof(float)` bytes (asserted).
 *
 * `vs_data` vertex stage data. May be NULL only if `vs_size` is 0.
 * `vs_size` size of `vs_data` in bytes.
 * `fs_data` fragment stage data. May be NULL only if `fs_size` is 0.
 * `fs_size` size of `fs_data` in bytes.
 */
BXR_API void bxr_painter_set_uniform(const void *vs_data,
                                     size_t vs_size,
                                     const void *fs_data,
                                     size_t fs_size);

/**
 * Clear the uniform data (no uniforms).
 */
BXR_API void bxr_painter_reset_uniform(void);

/**
 * Set the blend mode used by subsequent draws.
 */
BXR_API void bxr_painter_set_blendmode(bxr_blendmode_e blend_mode);

/**
 * Reset the current blend mode to the default blend mode (no blending).
 */
BXR_API void bxr_painter_reset_blendmode(void);

/**
 * Set the color used by subsequent draws.
 */
BXR_API void bxr_painter_set_color(bxr_color_t color);

/**
 * Get the current draw color.
 *
 * `return` the color set by `bxr_painter_set_color()`, white by default.
 */
BXR_API bxr_color_t bxr_painter_get_color(void);

/**
 * Reset current color to the default color (white).
 */
BXR_API void bxr_painter_reset_color(void);

/**
 * Bind an image to a texture channel for subsequent draws.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 * `image` the image to bind. Ownership stays with the caller.
 */
BXR_API void bxr_painter_set_image(int channel, bxr_image_t image);

/**
 * Unbind the image from a texture channel (no texture).
 *
 * This is the default for every channel except 0.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 */
BXR_API void bxr_painter_unset_image(int channel);

/**
 * Bind the builtin 1x1 white image to a texture channel.
 *
 * This is the default for channel 0.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 */
BXR_API void bxr_painter_reset_image(int channel);

/**
 * Bind a sampler to a texture channel for subsequent draws.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 * `sampler` the sampler to bind. Ownership stays with the caller.
 */
BXR_API void bxr_painter_set_sampler(int channel, bxr_sampler_t *sampler);

/**
 * Unbind the sampler from a texture channel (no sampler).
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 */
BXR_API void bxr_painter_unset_sampler(int channel);

/**
 * Bind the builtin nearest sampler to a texture channel (the default).
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`.
 */
BXR_API void bxr_painter_reset_sampler(int channel);

/**
 * Restrict subsequent draws to a rectangle.
 *
 * Coordinates are in virtual-resolution pixels, relative to the viewport, and
 * ignore the projection and transform.
 *
 * `x` and `y` the top-left corner.
 * `w` and `h` the size.
 */
BXR_API void bxr_painter_scissor(int x, int y, int w, int h);

/**
 * Remove the clipping rectangle (draw to the whole frame).
 */
BXR_API void bxr_painter_reset_scissor(void);

/**
 * Reset viewport, scissor, projection, transform, blend mode, color, uniform
 * and pipeline to their defaults.
 */
BXR_API void bxr_painter_reset_state(void);

/**
 * Fill the whole frame with the current color.
 *
 * Ignores projection and transform, but honors the scissor and the current
 * blend mode.
 */
BXR_API void bxr_painter_clear(void);

/**
 * Draw raw vertices as the given primitive.
 *
 * `primitive_type` how vertices are assembled.
 * `vertices` the vertex array. Must not be NULL if `vertices_count` > 0.
 * `vertices_count` number of elements in `vertices`.
 */
BXR_API void bxr_painter_draw(bxr_primitive_e primitive_type,
                              const bxr_vertex_t *vertices,
                              Uint32 vertices_count);

/**
 * Draw points with the current color.
 *
 * `points` the point array. Must not be NULL if `count` > 0.
 * `count` number of points.
 */
BXR_API void bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count);

/**
 * Draw a single point with the current color.
 */
BXR_API void bxr_painter_draw_point(bxr_vec2_t point);

/**
 * Draw independent line segments with the current color.
 *
 * `lines` the segment array. Must not be NULL if `count` > 0.
 * `count` number of segments.
 */
BXR_API void bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count);

/**
 * Draw a single line segment with the current color.
 */
BXR_API void bxr_painter_draw_line(bxr_line_t line);

/**
 * Draw a polyline joining consecutive points with the current color.
 *
 * `points` the point array. Must not be NULL if `count` > 0.
 * `count` number of points
 */
BXR_API void bxr_painter_draw_lines_strip(const bxr_vec2_t *points,
                                          Uint32 count);

/**
 * Draw filled triangles with the current color.
 *
 * `triangles` the triangle array. Must not be NULL if `count` > 0.
 * `count` number of triangles.
 */
BXR_API void bxr_painter_draw_filled_triangles(const bxr_triangle_t *triangles,
                                               Uint32 count);

/**
 * Draw a single filled triangle with the current color.
 */
BXR_API void bxr_painter_draw_filled_triangle(bxr_triangle_t triangle);

/**
 * Draw a filled triangle strip with the current color.
 *
 * `points` the strip vertices. Must not be NULL if `count` > 0.
 * `count` number of points.
 */
BXR_API void bxr_painter_draw_filled_triangles_strip(const bxr_vec2_t *points,
                                                     Uint32 count);

/**
 * Draw filled rectangles with the current color.
 *
 * `rects` the rectangle array. Must not be NULL if `count` > 0.
 * `count` number of rectangles.
 */
BXR_API void bxr_painter_draw_filled_rects(const bxr_rect_t *rects,
                                           Uint32 count);

/**
 * Draw a single filled rectangle with the current color.
 */
BXR_API void bxr_painter_draw_filled_rect(bxr_rect_t rect);

/**
 * Draw rectangles textured from the image bound to a channel, tinted by the
 * current color.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`; an
 * image must be bound to it.
 * `rects` the rectangle array. Must not be NULL if `count` > 0.
 * `count` number of rectangles.
 */
BXR_API void bxr_painter_draw_textured_rects(int channel,
                                             const bxr_textured_rect_t *rects,
                                             Uint32 count);

/**
 * Draw a single rectangle textured from the image bound to a channel, tinted
 * by the current color.
 *
 * `channel` the texture channel, in `[0, BXR_PAINTER_MAX_TEXTURE_SLOTS)`; an
 * image must be bound to it.
 * `rect` the rectangle to draw.
 */
BXR_API void bxr_painter_draw_textured_rect(int channel,
                                            bxr_textured_rect_t rect);

#endif // BXR_PAINTER_H_
