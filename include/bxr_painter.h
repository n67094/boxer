/**
 * File `bxr_painter.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Painter System for Rendering Graphics
 *
 * Allow to render 2D graphics using a simple API.
 *
 * Make sure to call `bxr_painter_begin`, `bxr_painter_flush`, and
 * `bxr_painter_end` in the correct order for each frame to issue draw calls.
 *
 * You can have multiple `bxr_painter_begin` and `bxr_painter_end` as long as
 * they don't intertwined (i.e. you can't call `bxr_painter_begin` twice without
 * calling `bxr_painter_end` in between).
 */

#ifndef BXR_PAINTER_H_
#define BXR_PAINTER_H_

#include <SDL3/SDL.h>

#include "bxr_color.h"
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

/**
 * ## Internal API
 */

/**
 * `swapchain_texture` is the new swapchain texture to use for the current
 * frame.
 */
void bxr_painter_update_swapchain_texture(SDL_GPUTexture *swapchain_texture);

/**
 * `cmd_buffer` is the new command buffer to use for the current frame.
 */
void bxr_painter_update_command_buffer(SDL_GPUCommandBuffer *cmd_buffer);

/**
 * Setup the painter system with the given description.
 *
 * `desc` is the description of the painter system to setup.
 */
void bxr_painter_setup(bxr_painter_desc_t *desc);

/**
 * Shutdown the painter system.
 */
void bxr_painter_shutdown(void);

/**
 * ## Public API
 */

/**
 * Begin recording draw calls for the current frame.
 *
 * `width` and `height` are the dimensions of the current swapchain texture.
 */
void bxr_painter_begin(int width, int height);

/**
 * Flush the recorded draw calls to the GPU.
 */
void bxr_painter_flush(void);

/**
 * End recording draw calls for the current frame. Must be called after
 * `bxr_painter_begin` and `bxr_painter_flush`.
 */
void bxr_painter_end(void);

/**
 * Set the coordinate space boundaries in the current viewport.
 */
void
bxr_painter_set_projection(float left, float right, float bottom, float top);

/**
 * Reset the projection to the default coordinate space, which is the
 * coordinate of the current viewport.
 */
void bxr_painter_reset_projection(void);

/**
 * Save the current transform matrix on the transform stack. To be pop later
 * with `bxr_pop_transform`.
 */
void bxr_painter_push_transform(void);

/**
 * Restore the transform matrix from the top of the transform stack. Must be
 * called after a call to `bxr_push_transform` to restore the transform matrix
 * to the state it was before the push.
 */
void bxr_painter_pop_transform(void);

/**
 * Reset the current transform matrix to the default state (identity, no
 * transformation).
 */
void bxr_painter_reset_transform(void);

/**
 * Translate the 2D coordinate space.
 *
 * `x` and `y` are the translation amounts in the x and y directions,
 * respectively.
 */
void bxr_painter_translate(float x, float y);

/**
 * Rotate the 2D coordinate space around the origin.
 *
 * `angle` is the rotation angle in radians.
 */
void bxr_painter_rotate(float angle);

/**
 * Rotate the 2D coordinate space around a point.
 *
 * `angle` is the rotation angle in radians.
 *
 * `ax` and `ay` are the coordinates of the point to rotate around.
 */
void bxr_painter_rotate_at(float angle, float ax, float ay);

/**
 * Scale the 2D coordinate space around the origin.
 *
 * `sx` and `sy` are the scaling factors in the x and y directions,
 * respectively.
 */
void bxr_painter_scale(float sx, float sy);

/**
 * Scale the 2D coordinate space around a point.
 *
 * `sx` and `sy` are the scaling factors in the x and y directions,
 * respectively.
 *
 * `ax` and `ay` are the coordinates of the point to scale around.
 */
void bxr_painter_scale_at(float sx, float sy, float ax, float ay);

/**
 * Set the current graphics pipeline.
 */
void bxr_painter_set_pipeline(bxr_pipeline_t pipeline);

/**
 * Reset the graphics pipeline to the default pipeline builtin pipeline.
 */
void bxr_painter_reset_pipeline(void);

/**
 * Set uniform data for the current pipeline.
 *
 * `vs_data` is a pointer to the uniform data for the vertex shader stage.
 *
 * `vs_size` is the size of the uniform data for the vertex shader stage in
 * bytes.
 *
 * `fs_data` is a pointer to the uniform data for the fragment shader stage.
 *
 * `fs_size` is the size of the uniform data for the fragment shader stage in
 * bytes.
 */
void bxr_painter_set_uniform(const void *vs_data,
                             size_t vs_size,
                             const void *fs_data,
                             size_t fs_size);

/**
 * Reset uniform data to the default state (current state color).
 */
void bxr_painter_reset_uniform(void);

/**
 * Set the current blend mode.
 */
void bxr_painter_set_blendmode(bxr_blendmode_e blend_mode);

/**
 * Reset the current blend mode to the default blend mode (no blending).
 */
void bxr_painter_reset_blendmode(void);

/**
 * Set current color.
 */
void bxr_painter_set_color(bxr_color_t color);

/**
 * Get the current color.
 */
bxr_color_t bxr_painter_get_color(void);

/**
 * Reset current color to the default color (white).
 */
void bxr_painter_reset_color(void);

/**
 * Set current bound image in a texture channel.
 *
 * `channel` is the texture channel to bind the image to.
 *
 * `image` is the image to bind to the texture channel.
 */
void bxr_painter_set_image(int channel, bxr_image_t image);

/**
 * Remove current bound image from a texture channel (no texture).
 *
 * `channel` is the texture channel to unbind the image from.
 */
void bxr_ainter_unset_image(int channel);

/**
 * Reset current bound image in a texture channel to the default (white
 * texture).
 *
 * `channel` is the texture channel to reset the image in.
 */
void bxr_painter_reset_image(int channel);

/**
 * Set current bound sampler in a texture channel.
 *
 * `channel` is the texture channel to bind the sampler to.
 *
 * `sampler` is the sampler to bind to the texture channel.
 */
void bxr_painter_set_sampler(int channel, bxr_sampler_t *sampler);

/**
 * Remove current bound sampler from a texture channel (no sampler).
 *
 * `channel` is the texture channel to unbind the sampler from.
 */
void bxr_painter_unset_sampler(int channel);

/**
 * Reset current bound sampler in a texture channel to default (nearest
 * sampler).
 *
 * `channel` is the texture channel to reset the sampler in.
 */
void bxr_painter_reset_sampler(int channel);

/**
 * Set the screen are to draw to.
 *
 * `x` and `y` are the coordinates of the top-left corner of the viewport.
 *
 * `w` and `h` are the width and height of the viewport, respectively.
 */
void bxr_painter_viewport(int x, int y, int w, int h);

/**
 * Reset the viewport to default (0, 0, width, height).
 */
void bxr_painter_reset_viewport(void);

/**
 * Set the clipping rectangle in the viewport.
 *
 * `x` and `y` are the coordinates of the top-left corner of the clipping
 * rectangle.
 *
 * `w` and `h` are the width and height of the clipping rectangle, respectively.
 */
void bxr_painter_scissor(int x, int y, int w, int h);

/**
 * Reset the clipping rectangle to default (viewport bounds).
 */
void bxr_painter_reset_scissor(void);

/**
 * Reset all state to default.
 */
void bxr_painter_reset_state(void);

/**
 * Clear the current viewport with the current color.
 */
void bxr_painter_clear(void);

/**
 * Draw any primitive.
 *
 * `primitive_type` is the type of primitive to draw.
 *
 * `vertices` is a pointer to the vertex data for the primitive.
 *
 * `vertices_count` is the number of vertices in the vertex data.
 */
void bxr_painter_draw(bxr_primitive_e primitive_type,
                      const bxr_vertex_t *vertices,
                      Uint32 vertices_count);

/**
 * Draw points in batch.
 *
 * `points` is a pointer to the point data to draw.
 *
 * `count` is the number of points to draw.
 */
void bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count);

/**
 * Draw a single point.
 */
void bxr_painter_draw_point(bxr_vec2_t point);

/**
 * Draw lines in batch.
 *
 * `lines` is a pointer to the line data to draw.
 *
 * `count` is the number of lines to draw.
 */
void bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count);

/**
 * Draw a single line.
 */
void bxr_painter_draw_line(bxr_line_t line);

/**
 * Draw a stip of lines.
 *
 * `points` is a pointer to the point data to draw.
 *
 * `count` is the number of points to draw.
 */
void bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count);

/**
 * Draw triangles in batch.
 *
 * `triangles` is a pointer to the triangle data to draw.
 *
 * `count` is the number of triangles to draw.
 */
void bxr_painter_draw_filled_triangles(const bxr_triangle_t *triangles,
                                       Uint32 count);

/**
 * Draw a single triangle.
 *
 * `triangle` is the triangle data to draw.
 */
void bxr_painter_draw_filled_triangle(bxr_triangle_t triangle);

/**
 * Draw a strip of triangles.
 *
 * `points` is a pointer to the point data to draw.
 *
 * `count` is the number of points to draw.
 */
void bxr_painter_draw_filled_triangles_strip(const bxr_vec2_t *points,
                                             Uint32 count);

/**
 * Draw rectangles in batch.
 *
 * `rects` is a pointer to the rectangle data to draw.
 *
 * `count` is the number of rectangles to draw.
 */
void bxr_painter_draw_filled_rects(const bxr_rect_t *rects, Uint32 count);

/**
 * Draw a single rectangle.
 *
 * `rect` is the rectangle data to draw.
 */
void bxr_painter_draw_filled_rect(bxr_rect_t rect);

/**
 * Draw textured rectangles in batch.
 *
 * `channel` is the texture channel to use for the textured rectangles.
 *
 * `rects` is a pointer to the textured rectangle data to draw.
 *
 * `count` is the number of textured rectangles to draw.
 */
void bxr_painter_draw_textured_rects(int channel,
                                     const bxr_textured_rect_t *rects,
                                     Uint32 count);

/**
 * Draw a single textured rectangle.
 *
 * `channel` is the texture channel to use for the textured rectangle.
 *
 * `rect` is the textured rectangle data to draw.
 */
void bxr_painter_draw_textured_rect(int channel, bxr_textured_rect_t rect);

#endif // BXR_PAINTER_H_
