<a name="line-1"></a>
File `bxr_painter.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Painter System for Rendering Graphics

Allow to render 2D graphics using a simple API.

Make sure to call `bxr_painter_begin`, `bxr_painter_flush`, and
`bxr_painter_end` in the correct order for each frame to issue draw calls.

You can have multiple `bxr_painter_begin` and `bxr_painter_end` as long as
they don't intertwined (i.e. you can't call `bxr_painter_begin` twice without
calling `bxr_painter_end` in between).

<a name="line-46"></a>
## Internal API

<a name="line-50"></a>
### 🔹 void bxr_painter_update_swapchain_texture(SDL_GPUTexture *swapchain_texture)

```cpp
void bxr_painter_update_swapchain_texture(SDL_GPUTexture *swapchain_texture)
```

`swapchain_texture` is the new swapchain texture to use for the current
frame.

<a name="line-56"></a>
### 🔹 void bxr_painter_update_command_buffer(SDL_GPUCommandBuffer *cmd_buffer)

```cpp
void bxr_painter_update_command_buffer(SDL_GPUCommandBuffer *cmd_buffer)
```

`cmd_buffer` is the new command buffer to use for the current frame.

<a name="line-61"></a>
### 🔹 void bxr_painter_setup(bxr_painter_desc_t *desc)

```cpp
void bxr_painter_setup(bxr_painter_desc_t *desc)
```

Setup the painter system with the given description.

`desc` is the description of the painter system to setup.

<a name="line-68"></a>
### 🔹 void bxr_painter_shutdown(void)

```cpp
void bxr_painter_shutdown(void)
```

Shutdown the painter system.

<a name="line-73"></a>
## Public API

<a name="line-77"></a>
### 🔹 void bxr_painter_begin(int width, int height)

```cpp
void bxr_painter_begin(int width, int height)
```

Begin recording draw calls for the current frame.

`width` and `height` are the dimensions of the current swapchain texture.

<a name="line-84"></a>
### 🔹 void bxr_painter_flush(void)

```cpp
void bxr_painter_flush(void)
```

Flush the recorded draw calls to the GPU.

<a name="line-89"></a>
### 🔹 void bxr_painter_end(void)

```cpp
void bxr_painter_end(void)
```

End recording draw calls for the current frame. Must be called after
`bxr_painter_begin` and `bxr_painter_flush`.

<a name="line-95"></a>
### 🔧 void

```cpp
void
```

Set the coordinate space boundaries in the current viewport.

<a name="line-101"></a>
### 🔹 void bxr_painter_reset_projection(void)

```cpp
void bxr_painter_reset_projection(void)
```

Reset the projection to the default coordinate space, which is the
coordinate of the current viewport.

<a name="line-107"></a>
### 🔹 void bxr_painter_push_transform(void)

```cpp
void bxr_painter_push_transform(void)
```

Save the current transform matrix on the transform stack. To be pop later
with `bxr_pop_transform`.

<a name="line-113"></a>
### 🔹 void bxr_painter_pop_transform(void)

```cpp
void bxr_painter_pop_transform(void)
```

Restore the transform matrix from the top of the transform stack. Must be
called after a call to `bxr_push_transform` to restore the transform matrix
to the state it was before the push.

<a name="line-120"></a>
### 🔹 void bxr_painter_reset_transform(void)

```cpp
void bxr_painter_reset_transform(void)
```

Reset the current transform matrix to the default state (identity, no
transformation).

<a name="line-126"></a>
### 🔹 void bxr_painter_translate(float x, float y)

```cpp
void bxr_painter_translate(float x, float y)
```

Translate the 2D coordinate space.

`x` and `y` are the translation amounts in the x and y directions,
respectively.

<a name="line-134"></a>
### 🔹 void bxr_painter_rotate(float angle)

```cpp
void bxr_painter_rotate(float angle)
```

Rotate the 2D coordinate space around the origin.

`angle` is the rotation angle in radians.

<a name="line-141"></a>
### 🔹 void bxr_painter_rotate_at(float angle, float ax, float ay)

```cpp
void bxr_painter_rotate_at(float angle, float ax, float ay)
```

Rotate the 2D coordinate space around a point.

`angle` is the rotation angle in radians.

`ax` and `ay` are the coordinates of the point to rotate around.

<a name="line-150"></a>
### 🔹 void bxr_painter_scale(float sx, float sy)

```cpp
void bxr_painter_scale(float sx, float sy)
```

Scale the 2D coordinate space around the origin.

`sx` and `sy` are the scaling factors in the x and y directions,
respectively.

<a name="line-158"></a>
### 🔹 void bxr_painter_scale_at(float sx, float sy, float ax, float ay)

```cpp
void bxr_painter_scale_at(float sx, float sy, float ax, float ay)
```

Scale the 2D coordinate space around a point.

`sx` and `sy` are the scaling factors in the x and y directions,
respectively.

`ax` and `ay` are the coordinates of the point to scale around.

<a name="line-168"></a>
### 🔹 void bxr_painter_set_pipeline(bxr_pipeline_t pipeline)

```cpp
void bxr_painter_set_pipeline(bxr_pipeline_t pipeline)
```

Set the current graphics pipeline.

<a name="line-173"></a>
### 🔹 void bxr_painter_reset_pipeline(void)

```cpp
void bxr_painter_reset_pipeline(void)
```

Reset the graphics pipeline to the default pipeline builtin pipeline.

<a name="line-178"></a>
### 🔹 void bxr_painter_set_uniform(const void *vs_data,

```cpp
void bxr_painter_set_uniform(const void *vs_data,
```

Set uniform data for the current pipeline.

`vs_data` is a pointer to the uniform data for the vertex shader stage.

`vs_size` is the size of the uniform data for the vertex shader stage in
bytes.

`fs_data` is a pointer to the uniform data for the fragment shader stage.

`fs_size` is the size of the uniform data for the fragment shader stage in
bytes.

<a name="line-196"></a>
### 🔹 void bxr_painter_reset_uniform(void)

```cpp
void bxr_painter_reset_uniform(void)
```

Reset uniform data to the default state (current state color).

<a name="line-201"></a>
### 🔹 void bxr_painter_set_blendmode(bxr_blendmode_e blend_mode)

```cpp
void bxr_painter_set_blendmode(bxr_blendmode_e blend_mode)
```

Set the current blend mode.

<a name="line-206"></a>
### 🔹 void bxr_painter_reset_blendmode(void)

```cpp
void bxr_painter_reset_blendmode(void)
```

Reset the current blend mode to the default blend mode (no blending).

<a name="line-211"></a>
### 🔹 void bxr_painter_set_color(bxr_color_t color)

```cpp
void bxr_painter_set_color(bxr_color_t color)
```

Set current color.

<a name="line-216"></a>
### 🔹 bxr_color_t bxr_painter_get_color(void)

```cpp
bxr_color_t bxr_painter_get_color(void)
```

Get the current color.

<a name="line-221"></a>
### 🔹 void bxr_painter_reset_color(void)

```cpp
void bxr_painter_reset_color(void)
```

Reset current color to the default color (white).

<a name="line-226"></a>
### 🔹 void bxr_painter_set_image(int channel, bxr_image_t image)

```cpp
void bxr_painter_set_image(int channel, bxr_image_t image)
```

Set current bound image in a texture channel.

`channel` is the texture channel to bind the image to.

`image` is the image to bind to the texture channel.

<a name="line-235"></a>
### 🔹 void bxr_ainter_unset_image(int channel)

```cpp
void bxr_ainter_unset_image(int channel)
```

Remove current bound image from a texture channel (no texture).

`channel` is the texture channel to unbind the image from.

<a name="line-242"></a>
### 🔹 void bxr_painter_reset_image(int channel)

```cpp
void bxr_painter_reset_image(int channel)
```

Reset current bound image in a texture channel to the default (white
texture).

`channel` is the texture channel to reset the image in.

<a name="line-250"></a>
### 🔹 void bxr_painter_set_sampler(int channel, bxr_sampler_t *sampler)

```cpp
void bxr_painter_set_sampler(int channel, bxr_sampler_t *sampler)
```

Set current bound sampler in a texture channel.

`channel` is the texture channel to bind the sampler to.

`sampler` is the sampler to bind to the texture channel.

<a name="line-259"></a>
### 🔹 void bxr_painter_unset_sampler(int channel)

```cpp
void bxr_painter_unset_sampler(int channel)
```

Remove current bound sampler from a texture channel (no sampler).

`channel` is the texture channel to unbind the sampler from.

<a name="line-266"></a>
### 🔹 void bxr_painter_reset_sampler(int channel)

```cpp
void bxr_painter_reset_sampler(int channel)
```

Reset current bound sampler in a texture channel to default (nearest
sampler).

`channel` is the texture channel to reset the sampler in.

<a name="line-274"></a>
### 🔹 void bxr_painter_viewport(int x, int y, int w, int h)

```cpp
void bxr_painter_viewport(int x, int y, int w, int h)
```

Set the screen are to draw to.

`x` and `y` are the coordinates of the top-left corner of the viewport.

`w` and `h` are the width and height of the viewport, respectively.

<a name="line-283"></a>
### 🔹 void bxr_painter_reset_viewport(void)

```cpp
void bxr_painter_reset_viewport(void)
```

Reset the viewport to default (0, 0, width, height).

<a name="line-288"></a>
### 🔹 void bxr_painter_scissor(int x, int y, int w, int h)

```cpp
void bxr_painter_scissor(int x, int y, int w, int h)
```

Set the clipping rectangle in the viewport.

`x` and `y` are the coordinates of the top-left corner of the clipping
rectangle.

`w` and `h` are the width and height of the clipping rectangle, respectively.

<a name="line-298"></a>
### 🔹 void bxr_painter_reset_scissor(void)

```cpp
void bxr_painter_reset_scissor(void)
```

Reset the clipping rectangle to default (viewport bounds).

<a name="line-303"></a>
### 🔹 void bxr_painter_reset_state(void)

```cpp
void bxr_painter_reset_state(void)
```

Reset all state to default.

<a name="line-308"></a>
### 🔹 void bxr_painter_clear(void)

```cpp
void bxr_painter_clear(void)
```

Clear the current viewport with the current color.

<a name="line-313"></a>
### 🔹 void bxr_painter_draw(bxr_primitive_e primitive_type,

```cpp
void bxr_painter_draw(bxr_primitive_e primitive_type,
```

Draw any primitive.

`primitive_type` is the type of primitive to draw.

`vertices` is a pointer to the vertex data for the primitive.

`vertices_count` is the number of vertices in the vertex data.

<a name="line-326"></a>
### 🔹 void bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count)

```cpp
void bxr_painter_draw_points(const bxr_vec2_t *points, Uint32 count)
```

Draw points in batch.

`points` is a pointer to the point data to draw.

`count` is the number of points to draw.

<a name="line-335"></a>
### 🔹 void bxr_painter_draw_point(bxr_vec2_t point)

```cpp
void bxr_painter_draw_point(bxr_vec2_t point)
```

Draw a single point.

<a name="line-340"></a>
### 🔹 void bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count)

```cpp
void bxr_painter_draw_lines(const bxr_line_t *lines, Uint32 count)
```

Draw lines in batch.

`lines` is a pointer to the line data to draw.

`count` is the number of lines to draw.

<a name="line-349"></a>
### 🔹 void bxr_painter_draw_line(bxr_line_t line)

```cpp
void bxr_painter_draw_line(bxr_line_t line)
```

Draw a single line.

<a name="line-354"></a>
### 🔹 void bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count)

```cpp
void bxr_painter_draw_lines_strip(const bxr_vec2_t *points, Uint32 count)
```

Draw a stip of lines.

`points` is a pointer to the point data to draw.

`count` is the number of points to draw.

<a name="line-363"></a>
### 🔹 void bxr_painter_draw_filled_triangles(const bxr_triangle_t *triangles,

```cpp
void bxr_painter_draw_filled_triangles(const bxr_triangle_t *triangles,
```

Draw triangles in batch.

`triangles` is a pointer to the triangle data to draw.

`count` is the number of triangles to draw.

<a name="line-373"></a>
### 🔹 void bxr_painter_draw_filled_triangle(bxr_triangle_t triangle)

```cpp
void bxr_painter_draw_filled_triangle(bxr_triangle_t triangle)
```

Draw a single triangle.

`triangle` is the triangle data to draw.

<a name="line-380"></a>
### 🔹 void bxr_painter_draw_filled_triangles_strip(const bxr_vec2_t *points,

```cpp
void bxr_painter_draw_filled_triangles_strip(const bxr_vec2_t *points,
```

Draw a strip of triangles.

`points` is a pointer to the point data to draw.

`count` is the number of points to draw.

<a name="line-390"></a>
### 🔹 void bxr_painter_draw_filled_rects(const bxr_rect_t *rects, Uint32 count)

```cpp
void bxr_painter_draw_filled_rects(const bxr_rect_t *rects, Uint32 count)
```

Draw rectangles in batch.

`rects` is a pointer to the rectangle data to draw.

`count` is the number of rectangles to draw.

<a name="line-399"></a>
### 🔹 void bxr_painter_draw_filled_rect(bxr_rect_t rect)

```cpp
void bxr_painter_draw_filled_rect(bxr_rect_t rect)
```

Draw a single rectangle.

`rect` is the rectangle data to draw.

<a name="line-406"></a>
### 🔹 void bxr_painter_draw_textured_rects(int channel,

```cpp
void bxr_painter_draw_textured_rects(int channel,
```

Draw textured rectangles in batch.

`channel` is the texture channel to use for the textured rectangles.

`rects` is a pointer to the textured rectangle data to draw.

`count` is the number of textured rectangles to draw.

<a name="line-419"></a>
### 🔹 void bxr_painter_draw_textured_rect(int channel, bxr_textured_rect_t rect)

```cpp
void bxr_painter_draw_textured_rect(int channel, bxr_textured_rect_t rect)
```

Draw a single textured rectangle.

`channel` is the texture channel to use for the textured rectangle.

`rect` is the textured rectangle data to draw.
