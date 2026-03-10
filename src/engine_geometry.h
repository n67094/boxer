/**
 * @file engine_geometry.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_GEOMETRY_H_
#define ENGINE_GEOMETRY_H_

#include "engine_math.h"

// Line
// --------------------------------------------------------------------------

typedef struct engine_line_s
{
  engine_vec2_t p0;
  engine_vec2_t p1;
} engine_line_t;

/**
 * @brief Construct a line
 */
#define engine_line_make(ax, ay, bx, by)                                       \
  ((const engine_line_t){ { ax, ay }, { bx, by } })

// Triangle
// --------------------------------------------------------------------------

typedef struct engine_triangle_s
{
  engine_vec2_t p0;
  engine_vec2_t p1;
  engine_vec2_t p2;
} engine_triangle_t;

/**
 * @brief Construct a triangle
 */
#define engine_triangle_make(ax, ay, bx, by, cx, cy)                           \
  ((const engine_triangle_t){ { ax, ay }, { bx, by }, { cx, cy } })

// Rectangle
// --------------------------------------------------------------------------

typedef struct engine_rect_s
{
  float x, y, w, h;
} engine_rect_t;

/**
 * @brief Construct a rectangle
 */
#define engine_rect_make(x, y, w, h) ((const engine_rect_t){ x, y, w, h })

/**
 * @brief Returns the area of the rectangle
 */
ENGINE_INLINE float
engine_rect_area(engine_rect_t rect)
{
  return rect.w * rect.h;
}

/**
 * @brief Returns true if the rectangles intersect
 */
ENGINE_INLINE bool
engine_rect_intersect(engine_rect_t r1, engine_rect_t r2)
{
  if (r1.x + r1.w < r2.x)
    return false;
  if (r2.x + r2.w < r1.x)
    return false;
  if (r1.y + r1.h < r2.y)
    return false;
  if (r2.y + r2.h < r1.y)
    return false;

  return true;
}

/**
 * @brief Returns true if r1 completely contains r2
 */
ENGINE_INLINE bool
engine_rect_contains(engine_rect_t r1, engine_rect_t r2)
{
  if (r2.x < r1.x)
    return false;
  if (r2.x + r2.w > r1.x + r1.w)
    return false;
  if (r2.y < r1.y)
    return false;
  if (r2.y + r2.h > r1.y + r1.h)
    return false;

  return true;
}

/**
 * @brief Returns true if the rectangles are adjacent (share a side)
 */
ENGINE_INLINE bool
engine_rect_adjacent(engine_rect_t r1, engine_rect_t r2)
{
  return (r2.x - (r1.x + r1.w) == 0 || // Check if r2 is to the right of r1
          r1.x - (r2.x + r2.w) == 0 || // Check if r2 is to the left of r1
          r2.y - (r1.y + r1.h) == 0 || // Check if r2 is below r1
          r1.y - (r2.y + r2.h) == 0);  // Check if r2 is above r1
}

// Region
// --------------------------------------------------------------------------

typedef struct engine_region_s
{
  float x0, y0, x1, y1;
} engine_region_t;

/**
 * @brief Construct a region
 */
#define engine_region_make(x0, y0, x1, y1)                                     \
  ((const engine_region_t){ x0, y0, x1, y1 })

// Textured rectangle
// --------------------------------------------------------------------------

typedef struct engine_textured_rect_s
{
  engine_rect_t dst;
  engine_rect_t src;
} engine_textured_rect_t;

/**
 * @brief Construct a textured rectangle
 */
#define engine_textured_rect_make(dst, src)                                    \
  ((const engine_textured_rect_t){ dst, src })

#endif // ENGINE_GEOMETRY_H_
