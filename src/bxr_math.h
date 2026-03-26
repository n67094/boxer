/**
 * @file bxr_math.h
 *
 * Thanks to empyreanx (pico_headers) for the math utilities inspiration.
 *
 * @brief Mathematical utilities.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_MATH_H_
#define BXR_MATH_H_

#include <float.h>
#include <math.h>

#include <SDL3/SDL.h>

#include "bxr_defs.h"

#define BXR_EPSILON 1e-5f

#define BXR_PI 3.14159265359f
#define BXR_PI2 (2.0f * BXR_PI)
#define BXR_E 2.71828182845f

#define FLOAT_MIN FLT_MIN
#define FLOAT_MAX FLT_MAX

#define bxr_sqrt sqrtf
#define bxr_cos cosf
#define bxr_sin sinf
#define bxr_acos acosf
#define bxr_asin asinf
#define bxr_atan2 atan2f
#define bxr_abs fabsf
#define bxr_fmod fmodf
#define bxr_exp expf
#define bxr_pow powf
#define bxr_floor floorf
#define bxr_ceil ceilf
#define bxr_log2 log2f
#define bxr_max fmaxf
#define bxr_min fminf

typedef struct bxr_vec2_s
{
  float x, y;
} bxr_vec2_t;

typedef struct bxr_vec3_s
{
  float x, y, z;
} bxr_vec3_t;

// Scalar functions and macros
// -------------------------------------------------------------------------

#define BXR_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define BXR_MAX(a, b) (((a) > (b)) ? (a) : (b))

/**
 * @brief Clamps the value to the given range
 */
inline float
bxr_clamp(float val, float min, float max)
{
  return ((val < min) ? min : ((val > max) ? max : val));
}

/**
 * @brief Computes the sign of the number
 *
 * @returns:
 * -1 if `val` is less than zero
 *  0 if `val` is equal to zero
 *  1 if `val` is greater than zero
 */
BXR_INLINE float
bxr_sign(float val)
{
  return ((0 == val) ? 0.0f : ((val > 0.0f) ? 1.0f : -1.0f));
}

/**
 * @brief Returns `true` if the values are within epsilon of one another
 */
BXR_INLINE bool
bxr_equal(float c1, float c2)
{
  return bxr_abs(c1 - c2) < BXR_EPSILON;
}

/**
 * @brief Linearly interpolates the two values
 *
 * @param step A number in [0, 1] that specifies the position between the values
 *
 */
BXR_INLINE float
bxr_lerp(float a, float b, float step)
{
  return a + (b - a) * step;
}

/**
 * @brief Linearly interpolates between two angles
 *
 * @param angle1 The first angle in radians
 * @param angle2 The second angle in radians
 * @param step A number in [0, 1] that specifies the position between the
 * angles
 */
BXR_INLINE float
bxr_lerp_angle(float angle1, float angle2, float step)
{
  float diff = angle2 - angle1;

  if (diff < -BXR_PI)
    diff += BXR_PI2;
  else if (diff > BXR_PI)
    diff -= BXR_PI2;

  return angle1 + diff * step;
}

/**
 * @brief Clamps the angle to be in [0, 2 * BXR_PI]
 *
 * @param angle The angle in radians
 */
BXR_INLINE float
bxr_normalize_angle(float angle)
{
  while (angle >= BXR_PI2)
    angle -= BXR_PI2;

  while (angle < 0.0f)
    angle += BXR_PI2;

  return angle;
}

// Vectors 2
// -------------------------------------------------------------------------

/**
 * @brief Constructs a vector
 */
#define bxr_vec2_make(x, y) ((const bxr_vec2_t){ x, y })

/**
 * @brief Returns true if the vectors are equal (within epsilon)
 */
BXR_INLINE bool
bxr_vec2_equal(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_equal(v1.x, v2.x) && bxr_equal(v1.y, v2.y);
}

/**
 * @brief Adds two vectors
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_add(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(v1.x + v2.x, v1.y + v2.y);
}

/**
 * @brief Subtracts two vectors
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_sub(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(v1.x - v2.x, v1.y - v2.y);
}

/**
 * @brief Multiplies two vectors component-wise
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_mul(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(v1.x * v2.x, v1.y * v2.y);
}

/**
 * @brief Divides two vectors component-wise
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_div(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(v1.x / v2.x, v1.y / v2.y);
}

/**
 * @brief Scales a vector
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_scale(bxr_vec2_t v, float c)
{
  return bxr_vec2_make(v.x * c, v.y * c);
}

/**
 * @brief Dot product
 */
BXR_INLINE float
bxr_vec2_dot(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

/**
 * @brief Returns the square of the length of the vector
 */
BXR_INLINE float
bxr_vec2_len2(bxr_vec2_t v)
{
  return bxr_vec2_dot(v, v);
}

/**
 * @brief Returns the length of the vector
 */
BXR_INLINE float
bxr_vec2_len(bxr_vec2_t v)
{
  return bxr_sqrt(bxr_vec2_len2(v));
}

/**
 * @brief Normalizes a vector (sets its length to one)
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_normalize(bxr_vec2_t v)
{
  float c = bxr_vec2_len(v);

  if (c < BXR_EPSILON)
    return bxr_vec2_make(0.0f, 0.0f);
  else
    return bxr_vec2_scale(v, 1.0f / c);
}

/**
 * @brief Negates a vector (scales it by -1.0)
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_reflect(bxr_vec2_t v)
{
  return bxr_vec2_scale(v, -1.0f);
}

/**
 * @brief Construct a vector that is perpendicular to the specified vector
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_perp(bxr_vec2_t v)
{
  return bxr_vec2_make(-v.y, v.x);
}

/**
 * @brief A 2D analog of the 3D cross product
 */
BXR_INLINE float
bxr_vec2_cross(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t perp = bxr_vec2_perp(v1);
  return bxr_vec2_dot(perp, v2);
}

/**
 * @brief Returns the angle the vector with respect to the current basis
 */
BXR_INLINE float
bxr_vec2_angle(bxr_vec2_t v)
{
  return atan2(v.y, v.x);
}

/**
 * @brief Projects a vector onto another
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_proj(bxr_vec2_t v1, bxr_vec2_t v2)
{
  float d = bxr_vec2_dot(v1, v2) / bxr_vec2_dot(v2, v2);

  return bxr_vec2_scale(v2, d);
}

/**
 * @brief Returns the distance between the two vectors
 */
BXR_INLINE float
bxr_vec2_dist(bxr_vec2_t v1, bxr_vec2_t v2)
{
  bxr_vec2_t v = bxr_vec2_sub(v1, v2);

  return bxr_vec2_len(v);
}

/**
 * @brief Linearly interpolates between two vectors
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
 * @brief Returns the zero vector
 */
#define bxr_vec2_t_zero() (bxr_vec2_make(0.0f, 0.0f))

/**
 * @brief Contructs a vector in polar coordinates
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_polar(float angle, float len)
{
  return bxr_vec2_make(len * bxr_cos(angle), len * bxr_sin(angle));
}

/**
 * @brief Computes the component-wise minimum of two vectors
 */
BXR_INLINE bxr_vec2_t
vec2_min(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(bxr_min(v1.x, v2.x), bxr_min(v1.y, v2.y));
}

/**
 * @brief Computes the component-wise maximum of two vectors
 */
BXR_INLINE bxr_vec2_t
vec2_max(bxr_vec2_t v1, bxr_vec2_t v2)
{
  return bxr_vec2_make(bxr_max(v1.x, v2.x), bxr_max(v1.y, v2.y));
}

/**
 * @brief Computes the component-wise floor of the specified vector
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_floor(bxr_vec2_t v)
{
  return bxr_vec2_make(bxr_floor(v.x), bxr_floor(v.y));
}

/**
 * @brief Computes the component-wise ceiling of the specified vector
 */
BXR_INLINE bxr_vec2_t
bxr_vec2_ceil(bxr_vec2_t v)
{
  return bxr_vec2_make(bxr_ceil(v.x), bxr_ceil(v.y));
}

// Vectors 3
// -------------------------------------------------------------------------

/**
 * @brief Constructs a vector
 */
#define bxr_vec3_make(x, y, z) ((const bxr_vec3_t){ x, y, z })

/**
 * @brief Returns true if the vectors are equal (within epsilon)
 */
BXR_INLINE bool
bxr_vec3_equal(bxr_vec3_t v1, bxr_vec3_t v2)
{
  return bxr_equal(v1.x, v2.x) && bxr_equal(v1.y, v2.y)
         && bxr_equal(v1.z, v2.z);
}

/**
 * @brief Adds two vectors
 */
BXR_INLINE bxr_vec3_t
bxr_vec3_add(bxr_vec3_t v1, bxr_vec3_t v2)
{
  return bxr_vec3_make(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

/**
 * @brief Subtracts two vectors
 */
BXR_INLINE bxr_vec3_t
bxr_vec3_sub(bxr_vec3_t v1, bxr_vec3_t v2)
{
  return bxr_vec3_make(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

/**
 * @brief Multiplies two vectors component-wise
 */
BXR_INLINE bxr_vec3_t
bxr_vec3_mul(bxr_vec3_t v1, bxr_vec3_t v2)
{
  return bxr_vec3_make(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

/**
 * @brief Divides two vectors component-wise
 */
BXR_INLINE bxr_vec3_t
bxr_vec3_div(bxr_vec3_t v1, bxr_vec3_t v2)
{
  return bxr_vec3_make(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

// Line
// --------------------------------------------------------------------------

typedef struct bxr_line_s
{
  bxr_vec2_t p0;
  bxr_vec2_t p1;
} bxr_line_t;

/**
 * @brief Construct a line
 */
#define bxr_line_make(ax, ay, bx, by)                                          \
  ((const bxr_line_t){ { ax, ay }, { bx, by } })

// Triangle
// --------------------------------------------------------------------------

typedef struct bxr_triangle_s
{
  bxr_vec2_t p0;
  bxr_vec2_t p1;
  bxr_vec2_t p2;
} bxr_triangle_t;

/**
 * @brief Construct a triangle
 */
#define bxr_triangle_make(ax, ay, bx, by, cx, cy)                              \
  ((const bxr_triangle_t){ { ax, ay }, { bx, by }, { cx, cy } })

// Rectangle
// --------------------------------------------------------------------------

typedef struct bxr_rect_s
{
  float x, y, w, h;
} bxr_rect_t;

/**
 * @brief Construct a rectangle
 */
#define bxr_rect_make(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })

/**
 * @brief Returns the area of the rectangle
 */
BXR_INLINE float
bxr_rect_area(bxr_rect_t rect)
{
  return rect.w * rect.h;
}

/**
 * @brief Returns true if the rectangles intersect (overlap)
 */
BXR_INLINE bool
bxr_rect_intersect(bxr_rect_t r1, bxr_rect_t r2)
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
 * @brief Returns true if the rectangles are adjacent (share a side)
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
 * @brief Return a rectangle that is expanded by the specified amount in all
 * directions
 */
BXR_INLINE bxr_rect_t
bxr_rect_expand(bxr_rect_t rect, float amount)
{
  return bxr_rect_make(rect.x - amount,
                       rect.y - amount,
                       rect.w + 2 * amount,
                       rect.h + 2 * amount);
}

/**
 * @brief Return a rectangle that is contracted by the specified amount in all
 * directions
 */
bxr_rect_t BXR_INLINE
bxr_rect_contract(bxr_rect_t rect, float amount)
{
  return bxr_rect_make(rect.x + amount,
                       rect.y + amount,
                       rect.w - 2 * amount,
                       rect.h - 2 * amount);
}

// Textured rectangle
// --------------------------------------------------------------------------

typedef struct bxr_textured_rect_s
{
  bxr_rect_t dst;
  bxr_rect_t src;
} bxr_textured_rect_t;

/**
 * @brief Construct a textured rectangle
 */
#define bxr_textured_rect_make(dst, src)                                       \
  ((const bxr_textured_rect_t){ dst, src })

// Region
// --------------------------------------------------------------------------

typedef struct bxr_region_s
{
  float x0, y0, x1, y1;
} bxr_region_t;

/**
 * @brief Construct a region
 */
#define bxr_region_make(x0, y0, x1, y1) ((const bxr_region_t){ x0, y0, x1, y1 })

#endif // BXR_MATH_H_
