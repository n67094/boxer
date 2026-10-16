/**
 * File `bxr_math.h`.
 *
 * # Boxer's Math Utilities.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
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

/**
 * Aliases for the SDL math functions, see the SDL documentation for details.
 */
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
 * Computes the sign of a number.
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
 * Compares two floats with a tolerance of `BXR_EPSILON`.
 *
 * `return` `true` if the values are within epsilon of one another.
 */
BXR_INLINE bool
bxr_equal(float c1, float c2)
{
  float diff = bxr_fabsf(c1 - c2);

  if (diff <= BXR_EPSILON) // near zero, absolute tolerance
    return true;

  return diff <= BXR_EPSILON * bxr_max(bxr_fabsf(c1), bxr_fabsf(c2));
}

/**
 * Linearly interpolates between two values.
 *
 * `a` the start value, returned when `step` is 0.
 * `b` the end value, returned when `step` is 1.
 * `step` the position between the values, usually in [0, 1]; values outside
 * this range extrapolate.
 *
 * `return` the interpolated value.
 */
BXR_INLINE float
bxr_lerp(float a, float b, float step)
{
  return a + (b - a) * step;
}

/**
 * Linearly interpolates between two angles along the shortest arc.
 *
 * `angle1` the start angle in radians.
 * `angle2` the end angle in radians.
 * `step` the position between the angles, usually in [0, 1].
 *
 * `return` the interpolated angle in radians.
 */
BXR_INLINE float
bxr_lerp_angle(float angle1, float angle2, float step)
{
  float diff = bxr_fmodf(angle2 - angle1, BXR_PI2_F); // (-2pi, 2pi)

  if (diff > BXR_PI_F)
    diff -= BXR_PI2_F;
  else if (diff < -BXR_PI_F)
    diff += BXR_PI2_F;

  return angle1 + diff * step;
}

/**
 * Wraps an angle into [0, 2 * pi).
 *
 * `angle` the angle in radians.
 *
 * `return` the equivalent angle in [0, 2 * pi).
 */
BXR_INLINE float
bxr_normalize_angle(float angle)
{
  float a = bxr_fmodf(angle, BXR_PI2_F);

  // If the input is negative, add 2*pi to wrap it into [0, 2*pi)
  if (a < 0.0f)
    a += BXR_PI2_F;

  // If the input is exactly 2*pi, wrap it to 0
  if (a >= BXR_PI2_F)
    a = 0.0f;

  return a;
}

/**
 * ## Point
 */

/**
 * Constructs a `bxr_point_t` compound literal from `x` and `y`.
 */
#define bxr_point_create(x, y) ((const bxr_point_t){ x, y })

/**
 * ## 2D Vector
 */

/**
 * Constructs a `bxr_vec2_t` compound literal from `x` and `y`.
 */
#define bxr_vec2_create(x, y) ((const bxr_vec2_t){ x, y })

/**
 * Compares two vectors component-wise with `bxr_equal()`.
 *
 * `return` `true` if both components are within epsilon of one another.
 */
BXR_INLINE bool
bxr_vec2_equal(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_equal(v1.x, v2.x) && bxr_equal(v1.y, v2.y);
}

/**
 * `return` the component-wise sum `v1 + v2`.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_add(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x + v2.x, v1.y + v2.y);
}

/**
 * `return` the component-wise difference `v1 - v2`.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_sub(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x - v2.x, v1.y - v2.y);
}

/**
 * `return` the component-wise product `v1 * v2`.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_mul(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x * v2.x, v1.y * v2.y);
}

/**
 * `return` the component-wise quotient `v1 / v2`.
 *
 * **Note**: a zero component in `v2` yields an infinite or NaN component.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_div(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(v1.x / v2.x, v1.y / v2.y);
}

/**
 * `return` `v` with both components multiplied by `c`.
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
 * `return` the squared length of the vector, cheaper than `bxr_vec2_len()`
 * when only comparing lengths.
 */
BXR_INLINE float
bxr_vec2_len2(bxr_vec2_t v)
{
  return bxr_vec2_dot(v, v);
}

/**
 * `return` the length (magnitude) of the vector.
 */
BXR_INLINE float
bxr_vec2_len(bxr_vec2_t v)
{
  return bxr_sqrtf(bxr_vec2_len2(v));
}

/**
 * `return` the unit vector with the same direction as `v`, or the zero vector
 * if the length of `v` is below `BXR_EPSILON`.
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
 * `return` the negation of the vector (`-v`).
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_negate(bxr_vec2_t v)
{
  return bxr_vec2_scale(v, -1.0f);
}

/**
 * `return` `v` rotated by 90 degrees counter-clockwise (`(-y, x)`).
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_perp(bxr_vec2_t v)
{
  return bxr_vec2_create(-v.y, v.x);
}

/**
 * Computes the 2D cross product (the z component of the 3D cross product).
 *
 * `return` `v1.x * v2.y - v1.y * v2.x`; positive if `v2` is counter-clockwise
 * from `v1`, negative if clockwise, 0 if they are parallel.
 */
BXR_INLINE float
bxr_vec2_cross(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t perp = bxr_vec2_perp(v1);
  return bxr_vec2_dot(perp, v2);
}

/**
 * `return` the angle in radians between the vector and the positive x axis,
 * in [-pi, pi].
 */
BXR_INLINE float
bxr_vec2_angle(bxr_vec2_t v)
{
  return bxr_atan2f(v.y, v.x);
}

/**
 * `return` the projection of `v1` onto `v2`.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_proj(bxr_vec2_t v1, bxr_vec2_t v2)
{
  float len2 = bxr_vec2_len2(v2);

  if (len2 < BXR_EPSILON * BXR_EPSILON)
    return (bxr_vec2_t){ 0.0f, 0.0f };

  float d = bxr_vec2_dot(v1, v2) / bxr_vec2_dot(v2, v2);

  return bxr_vec2_scale(v2, d);
}

/**
 * `return` the distance between the two points.
 */
BXR_INLINE float
bxr_vec2_dist(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t v = bxr_vec2_sub(v1, v2);

  return bxr_vec2_len(v);
}

/**
 * Linearly interpolates between two vectors, component-wise.
 *
 * `step` the position between `v1` and `v2`, usually in [0, 1].
 *
 * `return` the interpolated vector.
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
 * `return` the zero vector.
 */
#define bxr_vec2_zero() (bxr_vec2_create(0.0f, 0.0f))

/**
 * Builds a vector from polar coordinates.
 *
 * `angle` the angle in radians from the positive x axis.
 * `len` the length of the vector.
 *
 * `return` the vector in cartesian coordinates.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_polar(float angle, float len)
{
  return bxr_vec2_create(len * bxr_cosf(angle), len * bxr_sinf(angle));
}

/**
 * `return` the component-wise minimum of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_min(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(bxr_min(v1.x, v2.x), bxr_min(v1.y, v2.y));
}

/**
 * `return` the component-wise maximum of two vectors.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_max(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_create(bxr_max(v1.x, v2.x), bxr_max(v1.y, v2.y));
}

/**
 * `return` the component-wise floor of the specified vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_floor(bxr_vec2_t v)
{
  return bxr_vec2_create(bxr_floorf(v.x), bxr_floorf(v.y));
}

/**
 * `return` the component-wise ceiling of the specified vector.
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_ceil(bxr_vec2_t v)
{
  return bxr_vec2_create(bxr_ceilf(v.x), bxr_ceilf(v.y));
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
 * Constructs a `bxr_line_t` compound literal from the segment `(ax, ay)` to
 * `(bx, by)`.
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
 * Constructs a `bxr_triangle_t` compound literal from its three vertices.
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
 * Constructs a `bxr_rect_t` compound literal from its top-left corner
 * `(x, y)`, width `w` and height `h`.
 */
#define bxr_rect_create(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })

/**
 * `return` the area of the rectangle.
 */
BXR_INLINE float
bxr_rect_area(bxr_rect_t rect)
{
  return rect.w * rect.h;
}

/**
 * `return` `true` if the rectangles overlap or touch.
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
 * `return` the intersection of the two rectangles. If they do not overlap, the
 * returned rectangle has an area of 0.
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
 * `return` `true` if `r1` completely contains `r2`, edges included.
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
 * `return` `true` if the rectangle contains the point, edges included.
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
 * `return` `true` if an edge of one rectangle lies on the same line as the
 * opposite edge of the other.
 */
BXR_INLINE bool
bxr_rect_adjacent(bxr_rect_t r1, bxr_rect_t r2)
{
  bool overlap_x = r1.x <= r2.x + r2.w && r2.x <= r1.x + r1.w;
  bool overlap_y = r1.y <= r2.y + r2.h && r2.y <= r1.y + r1.h;

  // Left or right edges touching
  if (overlap_y
      && (bxr_equal(r1.x + r1.w, r2.x) || bxr_equal(r2.x + r2.w, r1.x)))
    return true;

  // Top or bottom edges touching
  if (overlap_x
      && (bxr_equal(r1.y + r1.h, r2.y) || bxr_equal(r2.y + r2.h, r1.y)))
    return true;

  return false;
}

/**
 * `return` the rectangle grown by `amount` on each side, keeping its center.
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
 * `return` the rectangle shrunk by `amount` on each side, keeping its center.
 *
 * **Note**: the width or height becomes negative when `amount` is greater
 * than half of it.
 */
BXR_INLINE bxr_rect_t
bxr_rect_contract(bxr_rect_t rect, float amount)
{
  return bxr_rect_create(rect.x + amount,
                         rect.y + amount,
                         rect.w - 2 * amount,
                         rect.h - 2 * amount);
}

#endif // BXR_MATH_H_
