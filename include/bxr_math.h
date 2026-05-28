/**
 * File `bxr_math.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Mathematical and Geometric Utilities
 */

#ifndef BXR_MATH_H_
#define BXR_MATH_H_

#include <float.h>

#include <SDL3/SDL.h>

#include "bxr_defs.h"

#define BXR_PI_D SDL_PI_D
#define BXR_PI_F SDL_PI_F
#define BXR_PI2_F (2.0f * SDL_PI_F)
#define BXR_PI2_D (2.0f * SDL_PI_D)

#define BXR_EPSILON SDL_FLT_EPSILON
#define BXR_FLOAT_MIN FLT_MIN
#define BXR_FLOAT_MAX FLT_MAX

#define bxr_abs SDL_abs
#define bxr_min SDL_min
#define bxr_max SDL_max
#define bxr_clamp SDL_clamp
#define bxr_acos SDL_acos
#define bxr_acosf SDL_acosf
#define bxr_asin SDL_asin
#define bxr_asinf SDL_asinf
#define bxr_atan SDL_atan
#define bxr_atanf SDL_atanf
#define bxr_atan2 SDL_atan2
#define bxr_atan2f SDL_atan2f
#define bxr_ceil SDL_ceil
#define bxr_ceilf SDL_ceilf
#define bxr_copysign SDL_copysign
#define bxr_copysignf SDL_copysignf
#define bxr_cos SDL_cos
#define bxr_cosf SDL_cosf
#define bxr_exp SDL_exp
#define bxr_expf SDL_expf
#define bxr_fabs SDL_fabs
#define bxr_fabsf SDL_fabsf
#define bxr_floor SDL_floor
#define bxr_floorf SDL_floorf
#define bxr_trunc SDL_trunc
#define bxr_truncf SDL_truncf
#define bxr_fmod SDL_fmod
#define bxr_fmodf SDL_fmodf
#define bxr_fmodf SDL_fmodf
#define bxr_isinf SDL_isinf
#define bxr_isinff SDL_isinff
#define bxr_isnan SDL_isnan
#define bxr_isnanf SDL_isnanf
#define bxr_modf SDL_modf
#define bxr_modff SDL_modff
#define bxr_pow SDL_pow
#define bxr_powf SDL_powf
#define bxr_round SDL_round
#define bxr_roundf SDL_roundf
#define bxr_lround SDL_lround
#define bxr_lroundf SDL_lroundf
#define bxr_scalbn SDL_scalbn
#define bxr_scalbnf SDL_scalbnf
#define bxr_sin SDL_sin
#define bxr_sinf SDL_sinf
#define bxr_sqrt SDL_sqrt
#define bxr_sqrtf SDL_sqrtf
#define bxr_tan SDL_tan
#define bxr_tanf SDL_tanf

typedef struct bxr_vec2_s
{
  float x, y;
} bxr_vec2_t;

typedef bxr_vec2_t bxr_point_t;

/**
 * ## Scalar Functions and Macros
 */

/**
 * Computes the sign of the number.
 *
 * `return`:
 *
 * - -1 if `val` is less than zero;
 * - 0 if `val` is equal to zero;
 * - 1 if `val` is greater than zero.
 */
BXR_INLINE float
bxr_sign(float val)
{
  return ((0 == val) ? 0.0f : ((val > 0.0f) ? 1.0f : -1.0f));
}

/**
 * `return` `true` if the values are within epsilon of one another.
 */
BXR_INLINE bool
bxr_equal(float c1, float c2)
{
  return bxr_abs(c1 - c2) < BXR_EPSILON;
}

/**
 * Linearly interpolates the two values.
 *
 * `step` a number in [0, 1] that specifies the position between the values.
 *
 * `return` the interpolated value.
 */
BXR_INLINE float
bxr_lerp(float a, float b, float step)
{
  return a + (b - a) * step;
}

/**
 * Linearly interpolates between two angles.
 *
 * `angle1` The first angle in radians.
 *
 * `angle2` The second angle in radians.
 *
 * `step` a number in [0, 1] that specifies the position between the
 * angles.
 *
 * `return` the interpolated angle in radians.
 */
BXR_INLINE float
bxr_lerp_angle(float angle1, float angle2, float step)
{
  float diff = angle2 - angle1;

  if (diff < -BXR_PI_F)
    diff += BXR_PI2_F;
  else if (diff > BXR_PI_F)
    diff -= BXR_PI2_F;

  return angle1 + diff * step;
}

/**
 * Clamps the angle to be in [0, 2 * BXR_PI].
 *
 * `angle` The angle in radians.
 *
 * `return` the normalized angle in radians.
 */
BXR_INLINE float
bxr_normalize_angle(float angle)
{
  while (angle >= BXR_PI2_F)
    angle -= BXR_PI2_F;

  while (angle < 0.0f)
    angle += BXR_PI2_F;

  return angle;
}

/**
 * ## Point
 */

/**
 * Constructs a point.
 */
#define bxr_point_create(x, y) ((const bxr_point_t){ x, y })

/**
 * Vectors 2
 */

/**
 * Constructs a vector.
 */
#define bxr_vec2_create(x, y) ((const bxr_vec2_t){ x, y })

/**
 * `returns` true if the vectors are equal (within epsilon).
 */
BXR_INLINE bool
bxr_vec2_equal(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_equal(v1.x, v2.x) && bxr_equal(v1.y, v2.y);
}

/**
 * `retrurn` the sum of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_add(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x + v2.x, v1.y + v2.y);
}

/**
 * `return` the difference of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_sub(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x - v2.x, v1.y - v2.y);
}

/**
 *
 * `return` the product of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_mul(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x * v2.x, v1.y * v2.y);
}

/**
 * `return` the quotient of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_div(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x / v2.x, v1.y / v2.y);
}

/**
 * `return` the vector scaled by the specified scalar.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_scale(bxr_vec2_t v, float c)
{
  return bxr_vec2_create(v.x * c, v.y * c);
}

/**
 * `return` the dot product of two vectors.
 */
BXR_INLINE float
bxr_vec2_dot(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

/**
 * `return` the squared length of the vector.
 */
BXR_INLINE float
bxr_vec2_len2(bxr_vec2_t v)
{
  return bxr_vec2_dot(v, v);
}

/**
 * `return` the length of the vector (magnitude).
 */
BXR_INLINE float
bxr_vec2_len(bxr_vec2_t v)
{
  return bxr_sqrt(bxr_vec2_len2(v));
}

/**
 * `return` the normalized vector (vector with length 1). If the vector has zero
 * length, the zero vector is returned.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_normalize(bxr_vec2_t v)
{
  float c = bxr_vec2_len(v);

  if (c < BXR_EPSILON)
    return bxr_vec2_create(0.0f, 0.0f);
  else
    return bxr_vec2_scale(v, 1.0f / c);
}

/**
 * `return` the negation of the vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_reflect(bxr_vec2_t v)
{
  return bxr_vec2_scale(v, -1.0f);
}

/**
 * `return` a vector that is perpendicular to the specified vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_perp(bxr_vec2_t v)
{
  return bxr_vec2_create(-v.y, v.x);
}

/**
 * `return` A 2D analog of the 3D cross product.
 */
BXR_INLINE float
bxr_vec2_cross(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t perp = bxr_vec2_perp(v1);
  return bxr_vec2_dot(perp, v2);
}

/**
 * `return` the angle the vector with respect to the current basis.
 */
BXR_INLINE float
bxr_vec2_angle(bxr_vec2_t v)
{
  return bxr_atan2(v.y, v.x);
}

/**
 * `return` the projection of `v1` onto `v2`.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_proj(bxr_vec2_t v1, bxr_vec2_t v2)
{
  float d = bxr_vec2_dot(v1, v2) / bxr_vec2_dot(v2, v2);

  return bxr_vec2_scale(v2, d);
}

/**
 * `return` the distance between the two vectors.
 */
BXR_INLINE float
bxr_vec2_dist(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t v = bxr_vec2_sub(v1, v2);

  return bxr_vec2_len(v);
}

/**
 * `return` the linearly interpolates between two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_lerp(bxr_vec2_t v1, bxr_vec2_t v2, float step)
{
  bxr_vec2_t out;

  out.x = bxr_lerp(v1.x, v2.x, step);
  out.y = bxr_lerp(v1.y, v2.y, step);

  return out;
}

/**
 * `returns` the zero vector.
 */
#define bxr_vec2_t_zero() (bxr_vec2_create(0.0f, 0.0f))

/**
 * `return` a vector in polar coordinates.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_polar(float angle, float len)
{
  return bxr_vec2_create(len * bxr_cos(angle), len * bxr_sin(angle));
}

/**
 * `return` the component-wise minimum of two vectors.
 */
BXR_INLINE bxr_vec2_t
vec2_min(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(bxr_min(v1.x, v2.x), bxr_min(v1.y, v2.y));
}

/**
 * `return` the component-wise maximum of two vectors.
 */
BXR_INLINE bxr_vec2_t
vec2_max(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(bxr_max(v1.x, v2.x), bxr_max(v1.y, v2.y));
}

/**
 * `return` the component-wise floor of the specified vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_floor(bxr_vec2_t v)
{
  return bxr_vec2_create(bxr_floor(v.x), bxr_floor(v.y));
}

/**
 * `return` the component-wise ceiling of the specified vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_ceil(bxr_vec2_t v)
{
  return bxr_vec2_create(bxr_ceil(v.x), bxr_ceil(v.y));
}

/**
 * ## Line
 */

typedef struct bxr_line_s
{
  bxr_vec2_t a;
  bxr_vec2_t b;
} bxr_line_t;

/**
 * Construct a line
 */
#define bxr_line_create(ax, ay, bx, by)                                        \
  ((const bxr_line_t){ { ax, ay }, { bx, by } })

/**
 * ## Triangle
 */

typedef struct bxr_triangle_s
{
  bxr_vec2_t a;
  bxr_vec2_t b;
  bxr_vec2_t c;
} bxr_triangle_t;

/**
 * Construct a triangle
 */
#define bxr_triangle_create(ax, ay, bx, by, cx, cy)                            \
  ((const bxr_triangle_t){ { ax, ay }, { bx, by }, { cx, cy } })

/**
 * ## Rectangle
 */

typedef struct bxr_rect_s
{
  float x, y, w, h;
} bxr_rect_t;

/**
 * Construct a rectangle
 */
#define bxr_rect_create(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })

/**
 * `returns` the area of the rectangle
 */
BXR_INLINE float
bxr_rect_area(bxr_rect_t rect)
{
  return rect.w * rect.h;
}

/**
 * `returns` true if the rectangles overlap
 */
BXR_INLINE bool
bxr_rect_overlap(bxr_rect_t r1, bxr_rect_t r2)
{
  if (r1.x + r1.w < r2.x) // Check if r1 is fully to the left of r2
    return false;
  if (r2.x + r2.w < r1.x) // Check if r1 is fully to the right of r2
    return false;
  if (r1.y + r1.h < r2.y) // Check if r1 is fully above r2
    return false;
  if (r2.y + r2.h < r1.y) // Check if r1 is fully below r2
    return false;

  return true;
}

/**
 * `returns` the intersection of the two rectangles.
 *
 * If they don't overlap, `return` a rectangle starting at r2 x, y with zero
 * width and height.
 */
BXR_INLINE bxr_rect_t
bxr_rect_get_intersection(bxr_rect_t r1, bxr_rect_t r2)
{
  float x = bxr_max(r1.x, r2.x);
  float y = bxr_max(r1.y, r2.y);
  float w = bxr_min(r1.x + r1.w, r2.x + r2.w);
  float h = bxr_min(r1.y + r1.h, r2.y + r2.h);

  if (w < x)
    w = x;

  if (h < y)
    h = y;

  return bxr_rect_create(x, y, w - x, h - y);
}

/**
 * `return` true if r1 completely contains r2.
 */
BXR_INLINE bool
bxr_rect_contains(bxr_rect_t r1, bxr_rect_t r2)
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
 * `return` true if the rectangle contains the point.
 */
BXR_INLINE bool
bxr_rect_contains_point(bxr_rect_t rect, bxr_vec2_t point)
{
  if (point.x < rect.x)
    return false;
  if (point.x > rect.x + rect.w)
    return false;
  if (point.y < rect.y)
    return false;
  if (point.y > rect.y + rect.h)
    return false;

  return true;
}

/**
 * `return` true if the rectangles are adjacent (share a side).
 */
BXR_INLINE bool
bxr_rect_adjacent(bxr_rect_t r1, bxr_rect_t r2)
{
  return (r2.x - (r1.x + r1.w) == 0 || // Check if r2 is to the right of r1
          r1.x - (r2.x + r2.w) == 0 || // Check if r2 is to the left of r1
          r2.y - (r1.y + r1.h) == 0 || // Check if r2 is below r1
          r1.y - (r2.y + r2.h) == 0);  // Check if r2 is above r1
}

/**
 * `return` a rectangle that is expanded by the specified amount in all
 * directions.
 */
BXR_INLINE bxr_rect_t
bxr_rect_expand(bxr_rect_t rect, float amount)
{
  return bxr_rect_create(rect.x - amount,
                         rect.y - amount,
                         rect.w + 2 * amount,
                         rect.h + 2 * amount);
}

/**
 * `return` a rectangle that is contracted by the specified amount in all
 * directions.
 */
BXR_INLINE bxr_rect_t
bxr_rect_contract(bxr_rect_t rect, float amount)
{
  return bxr_rect_create(rect.x + amount,
                         rect.y + amount,
                         rect.w - 2 * amount,
                         rect.h - 2 * amount);
}

/**
 * ## Textured Rectangle
 */

typedef struct bxr_textured_rect_s
{
  bxr_rect_t src;
  bxr_rect_t dst;
} bxr_textured_rect_t;

/**
 * Construct a textured rectangle.
 */
#define bxr_textured_rect_create(src, dst)                                     \
  ((const bxr_textured_rect_t){ src, dst })

/**
 * ## Region
 */

typedef struct bxr_region_s
{
  float x0, y0, x1, y1;
} bxr_region_t;

/**
 * Construct a region
 */
#define bxr_region_create(x0, y0, x1, y1)                                      \
  ((const bxr_region_t){ x0, y0, x1, y1 })

#endif // BXR_MATH_H_
