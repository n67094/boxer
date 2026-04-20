#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include "bxr_painter.h"

void
bxr_painter_update_swapchain_texture(SDL_GPUTexture *swapchain_texture)
{
  SDL_GPUpdateSwapchainTexture(swapchain_texture);
}

void
bxr_painter_update_command_buffer(SDL_GPUCommandBuffer *cmd_buffer)
{
  SDL_GPUpdateCommandBuffer(cmd_buffer);
}

void
bxr_painter_setup(bxr_painter_desc_t *desc)
{
  SDL_assert(desc);
  SDL_assert(desc->window);
  SDL_assert(desc->gpu_device);

  SDL_GPDesc painter_desc = {
    .max_vertices = desc->max_vertices,
    .max_commands = desc->max_commands,
    .window       = desc->window,
    .gpu_device   = desc->gpu_device,
  };

  SDL_GPSetup(&painter_desc);
}

void
bxr_painter_shutdown()
{
  SDL_GPShutdown();
}

void
bxr_painter_begin(int width, int height)
{
  SDL_GPBegin(width, height);
}

void
bxr_painter_flush()
{
  SDL_GPFlush();
}

void
bxr_painter_end()
{
  SDL_GPEnd();
}

void
bxr_painter_set_projection(float left, float right, float bottom, float top)
{
  SDL_GPSetProjection(left, right, bottom, top);
}

void
bxr_painter_reset_projection(void)
{
  SDL_GPResetProjection();
}

void
bxr_push_transform(void)
{
  SDL_GPPushTransform();
}

void
bxr_pop_transform(void)
{
  SDL_GPPopTransform();
}

void
bxr_reset_transform(void)
{
  SDL_GPResetTransform();
}

void
bxr_translate(float x, float y)
{
  SDL_GPTranslate(x, y);
}

void
bxr_rotate(float angle)
{
  SDL_GPRotate(angle);
}

void
bxr_rotate_at(float angle, float ax, float ay)
{
  SDL_GPRotateAt(angle, ax, ay);
}

void
bxr_scale(float sx, float sy)
{
  SDL_GPScale(sx, sy);
}

void
bxr_scale_at(float sx, float sy, float ax, float ay)
{
  SDL_GPScaleAt(sx, sy, ax, ay);
}

void
bxr_set_pipeline(bxr_pipeline_t pipeline)
{
  SDL_GPSetPipeline(pipeline);
}

void
bxr_reset_pipeline(void)
{
  SDL_GPResetPipeline();
}

void
bxr_set_uniform(const void *vs_data,
                size_t vs_size,
                const void *fs_data,
                size_t fs_size)
{
  SDL_GPSetUniform(vs_data, vs_size, fs_data, fs_size);
}

void
bxr_reset_uniform(void)
{
  SDL_GPResetUniform();
}

void
bxr_set_blendmode(bxr_blendmode_e blend_mode)
{
  SDL_GPSetBlendMode((SDL_GPBlendMode)blend_mode);
}

void
bxr_reset_blendmode(void)
{
  SDL_GPResetBlendMode();
}

void
bxr_set_color(bxr_color_t color)
{

  SDL_Color sdl_color = {
    .r = color.r,
    .g = color.g,
    .b = color.b,
    .a = color.a,
  };

  SDL_GPSetColor(sdl_color);
}

bxr_color_t
bxr_get_color(void)
{
  SDL_Color sdl_color = SDL_GPGetColor();

  return (bxr_color_t){
    .r = sdl_color.r,
    .g = sdl_color.g,
    .b = sdl_color.b,
    .a = sdl_color.a,
  };
}

void
bxr_reset_color(void)
{
  SDL_GPResetColor();
}

void
bxr_set_image(int channel, bxr_image_t image)
{
  SDL_GPSetImage(channel, (SDL_GPImage)image);
}

void
bxr_unset_image(int channel)
{
  SDL_GPUnsetImage(channel);
}

void
bxr_reset_image(int channel)
{
  SDL_GPResetImage(channel);
}

void
bxr_unset_sampler(int channel)
{
  SDL_GPUnsetSampler(channel);
}

void
bxr_reset_sampler(int channel)
{
  SDL_GPResetSampler(channel);
}

void
bxr_viewport(int x, int y, int w, int h)
{
  SDL_GPViewport(x, y, w, h);
}

void
bxr_reset_viewport(void)
{
  SDL_GPResetViewport();
}

void
bxr_scissor(int x, int y, int w, int h)
{
  SDL_GPScissor(x, y, w, h);
}

void
bxr_reset_scissor(void)
{
  SDL_GPResetScissor();
}

void
bxr_reset_state(void)
{
  SDL_GPResetState();
}

void
bxr_clear(void)
{
  SDL_GPClear();
}

void
bxr_draw(bxr_primitive_e primitive_type,
         const bxr_vertex_t *vertices,
         Uint32 vertices_count)
{
  SDL_GPDraw((SDL_GPPrimitiveType)primitive_type,
             (SDL_GPVertex *)vertices,
             vertices_count);
}

void
bxr_draw_points(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawPoints((SDL_GPVec2 *)points, count);
}

void
bxr_draw_point(bxr_vec2_t point)
{
  SDL_GPVec2 gp_point = { .x = point.x, .y = point.y };

  SDL_GPDrawPoint(gp_point);
}

void
bxr_draw_lines(const bxr_line_t *lines, Uint32 count)
{
  SDL_GPDrawLines((SDL_GPLine *)lines, count);
}

void
bxr_draw_line(bxr_line_t line)
{
  SDL_GPLine gp_line = {
    .a = { .x = line.p0.x, .y = line.p0.y },
    .b = { .x = line.p1.x, .y = line.p1.y },
  };

  SDL_GPDrawLine(gp_line);
}

void
bxr_draw_lines_strip(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawLinesStrip((SDL_GPVec2 *)points, count);
}

void
bxr_draw_filled_triangles(const bxr_triangle_t *triangles, Uint32 count)
{
  SDL_GPDrawFilledTriangles((SDL_GPTriangle *)triangles, count);
}

void
bxr_draw_filled_triangle(bxr_triangle_t triangle)
{
  SDL_GPTriangle gp_triangle = {
    .a = { .x = triangle.p0.x, .y = triangle.p0.y },
    .b = { .x = triangle.p1.x, .y = triangle.p1.y },
    .c = { .x = triangle.p2.x, .y = triangle.p2.y },
  };

  SDL_GPDrawFilledTriangle(gp_triangle);
}

void
bxr_draw_filled_triangles_strip(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawFilledTrianglesStrip((SDL_GPVec2 *)points, count);
}

void
bxr_draw_filled_rects(const bxr_rect_t *rects, Uint32 count)
{
  SDL_GPDrawFilledRects((SDL_GPRect *)rects, count);
}

void
bxr_draw_filled_rect(bxr_rect_t rect)
{
  SDL_GPRect gp_rect = { .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h };

  SDL_GPDrawFilledRect(gp_rect);
}

void
bxr_draw_textured_rects(int channel,
                        const bxr_textured_rect_t *rects,
                        Uint32 count)
{
  SDL_GPDrawTexturedRects(channel, (SDL_GPTexturedRect *)rects, count);
}

void
bxr_draw_textured_rect(int channel, bxr_textured_rect_t rect)
{
  SDL_GPTexturedRect gp_rect = {
    .dst
    = { .x = rect.dst.x, .y = rect.dst.y, .w = rect.dst.w, .h = rect.dst.h },
    .src
    = { .x = rect.src.x, .y = rect.src.y, .w = rect.src.w, .h = rect.src.h },
  };

  SDL_GPDrawTexturedRect(channel, gp_rect);
}
