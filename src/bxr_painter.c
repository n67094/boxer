#include <SDL3/SDL.h>

#include <SDL_gp.h>

#include "bxr_assert.h"
#include "bxr_painter.h"

void
bxr_painter_setup(bxr_painter_desc_t *desc)
{
  BXR_ASSERT(desc);
  BXR_ASSERT(desc->window);
  BXR_ASSERT(desc->gpu_device);

  SDL_GPDesc painter_desc = {
    .max_vertices = desc->max_vertices,
    .max_commands = desc->max_commands,
    .window       = desc->window,
    .gpu_device   = desc->gpu_device,
  };

  SDL_GPSetup(&painter_desc);
}

void
bxr_painter_teardown()
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
bxr_painter_push_transform(void)
{
  SDL_GPPushTransform();
}

void
bxr_painter_pop_transform(void)
{
  SDL_GPPopTransform();
}

void
bxr_painter_reset_transform(void)
{
  SDL_GPResetTransform();
}

void
bxr_painter_translate(float x, float y)
{
  SDL_GPTranslate(x, y);
}

void
bxr_painter_rotate(float angle)
{
  SDL_GPRotate(angle);
}

void
bxr_painter_rotate_at(float angle, float ax, float ay)
{
  SDL_GPRotateAt(angle, ax, ay);
}

void
bxr_painter_scale(float sx, float sy)
{
  SDL_GPScale(sx, sy);
}

void
bxr_painter_scale_at(float sx, float sy, float ax, float ay)
{
  SDL_GPScaleAt(sx, sy, ax, ay);
}

void
bxr_painter_set_pipeline(bxr_pipeline_t pipeline)
{
  SDL_GPPipeline gp_pipeline = {
    .id = pipeline.id,
  };

  SDL_GPSetPipeline(gp_pipeline);
}

void
bxr_painter_reset_pipeline(void)
{
  SDL_GPResetPipeline();
}

void
bxr_painter_set_uniform(const void *vs_data,
                        size_t vs_size,
                        const void *fs_data,
                        size_t fs_size)
{
  SDL_GPSetUniform(vs_data, vs_size, fs_data, fs_size);
}

void
bxr_painter_reset_uniform(void)
{
  SDL_GPResetUniform();
}

void
bxr_painter_set_blendmode(bxr_blendmode_e blend_mode)
{
  SDL_GPSetBlendMode((SDL_GPBlendMode)blend_mode);
}

void
bxr_painter_reset_blendmode(void)
{
  SDL_GPResetBlendMode();
}

void
bxr_painter_set_color(bxr_color_t color)
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
bxr_painter_get_color(void)
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
bxr_painter_reset_color(void)
{
  SDL_GPResetColor();
}

void
bxr_painter_set_image(int channel, bxr_image_t image)
{
  SDL_GPImage gp_image = {
    .id = image.id,
  };

  SDL_GPSetImage(channel, gp_image);
}

void
bxr_painter_unset_image(int channel)
{
  SDL_GPUnsetImage(channel);
}

void
bxr_painter_reset_image(int channel)
{
  SDL_GPResetImage(channel);
}

void
bxr_painter_unset_sampler(int channel)
{
  SDL_GPUnsetSampler(channel);
}

void
bxr_painter_reset_sampler(int channel)
{
  SDL_GPResetSampler(channel);
}

void
bxr_painter_viewport(int x, int y, int w, int h)
{
  SDL_GPViewport(x, y, w, h);
}

void
bxr_painter_reset_viewport(void)
{
  SDL_GPResetViewport();
}

void
bxr_painter_scissor(int x, int y, int w, int h)
{
  SDL_GPScissor(x, y, w, h);
}

void
bxr_painter_reset_scissor(void)
{
  SDL_GPResetScissor();
}

void
bxr_painter_reset_state(void)
{
  SDL_GPResetState();
}

void
bxr_painter_clear(void)
{
  SDL_GPClear();
}

void
bxr_painter_draw(bxr_primitive_e primitive_type,
                 const bxr_vertex_t *vertices,
                 Uint32 vertices_count)
{
  SDL_GPDraw((SDL_GPPrimitiveType)primitive_type,
             (SDL_GPVertex *)vertices,
             vertices_count);
}

void
bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawPoints((SDL_GPVec2 *)points, count);
}

void
bxr_painter_draw_point(bxr_vec2_t point)
{
  SDL_GPVec2 gp_point = { .x = point.x, .y = point.y };

  SDL_GPDrawPoint(gp_point);
}

void
bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count)
{
  SDL_GPDrawLines((SDL_GPLine *)lines, count);
}

void
bxr_painter_draw_line(bxr_line_t line)
{
  SDL_GPLine gp_line = {
    .a = { .x = line.a.x, .y = line.a.y },
    .b = { .x = line.b.x, .y = line.b.y },
  };

  SDL_GPDrawLine(gp_line);
}

void
bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawLinesStrip((SDL_GPVec2 *)points, count);
}

void
bxr_painter_draw_filled_triangles(const bxr_triangle_t *triangles, Uint32 count)
{
  SDL_GPDrawFilledTriangles((SDL_GPTriangle *)triangles, count);
}

void
bxr_painter_draw_filled_triangle(bxr_triangle_t triangle)
{
  SDL_GPTriangle gp_triangle = {
    .a = { .x = triangle.a.x, .y = triangle.a.y },
    .b = { .x = triangle.b.x, .y = triangle.b.y },
    .c = { .x = triangle.c.x, .y = triangle.c.y },
  };

  SDL_GPDrawFilledTriangle(gp_triangle);
}

void
bxr_painter_draw_filled_triangles_strip(const bxr_vec2_t *points, Uint32 count)
{
  SDL_GPDrawFilledTrianglesStrip((SDL_GPVec2 *)points, count);
}

void
bxr_painter_draw_filled_rects(const bxr_rect_t *rects, Uint32 count)
{
  SDL_GPDrawFilledRects((SDL_GPRect *)rects, count);
}

void
bxr_painter_draw_filled_rect(bxr_rect_t rect)
{
  SDL_GPRect gp_rect = { .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h };

  SDL_GPDrawFilledRect(gp_rect);
}

void
bxr_painter_draw_textured_rects(int channel,
                                const bxr_textured_rect_t *rects,
                                Uint32 count)
{
  SDL_GPDrawTexturedRects(channel, (SDL_GPTexturedRect *)rects, count);
}

void
bxr_painter_draw_textured_rect(int channel, bxr_textured_rect_t rect)
{
  SDL_GPTexturedRect gp_rect = {
    .dst
    = { .x = rect.dst.x, .y = rect.dst.y, .w = rect.dst.w, .h = rect.dst.h },
    .src
    = { .x = rect.src.x, .y = rect.src.y, .w = rect.src.w, .h = rect.src.h },
  };

  SDL_GPDrawTexturedRect(channel, gp_rect);
}
