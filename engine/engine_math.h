/**
 * @file engine_math.h
 *
 * Thanks to empyreanx (pico_headers) for the math utilities inspiration.
 *
 * @brief Mathematical utilities.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_MATH_H_
#define ENGINE_MATH_H_

#include <float.h>
#include <math.h>

#include <SDL3/SDL.h>

#include "engine_defs.h"

#define ENGINE_EPSILON 1e-5f

#define ENGINE_PI 3.14159265359f
#define ENGINE_PI2 (2.0f * ENGINE_PI)
#define ENGINE_E 2.71828182845f

#define FLOAT_MIN FLT_MIN
#define FLOAT_MAX FLT_MAX

#define engine_sqrt sqrtf
#define engine_cos cosf
#define engine_sin sinf
#define engine_acos acosf
#define engine_asin asinf
#define engine_atan2 atan2f
#define engine_abs fabsf
#define engine_fmod fmodf
#define engine_exp expf
#define engine_pow powf
#define engine_floor floorf
#define engine_ceil ceilf
#define engine_log2 log2f
#define engine_max fmaxf
#define engine_min fminf

typedef struct engine_vec2_s
{
  float x, y;
} engine_vec2_t;

typedef struct engine_vec3_s
{
  float x, y, z;
} engine_vec3_t;

typedef struct engine_rect_s
{
  float x, y, w, h;
} engine_rect_t;

typedef struct engine_region_s
{
  float x0, y0, x1, y1;
} engine_region_t;

// Scalar functions and macros
// -------------------------------------------------------------------------

#define ENGINE_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ENGINE_MAX(a, b) (((a) > (b)) ? (a) : (b))

/**
 * @brief Clamps the value to the given range
 */
inline float
engine_clamp(float val, float min, float max)
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
ENGINE_INLINE float
engine_sign(float val)
{
  return ((0 == val) ? 0.0f : ((val > 0.0f) ? 1.0f : -1.0f));
}

/**
 * @brief Returns `true` if the values are within epsilon of one another
 */
ENGINE_INLINE bool
engine_equal(float c1, float c2)
{
  return engine_abs(c1 - c2) < ENGINE_EPSILON;
}

/**
 * @brief Linearly interpolates the two values
 *
 * @param step A number in [0, 1] that specifies the position between the values
 *
 */
ENGINE_INLINE float
engine_lerp(float a, float b, float step)
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
ENGINE_INLINE float
engine_lerp_angle(float angle1, float angle2, float step)
{
  float diff = angle2 - angle1;

  if (diff < -ENGINE_PI)
    diff += ENGINE_PI2;
  else if (diff > ENGINE_PI)
    diff -= ENGINE_PI2;

  return angle1 + diff * step;
}

/**
 * @brief Clamps the angle to be in [0, 2 * ENGINE_PI]
 *
 * @param angle The angle in radians
 */
ENGINE_INLINE float
engine_normalize_angle(float angle)
{
  while (angle >= ENGINE_PI2)
    angle -= ENGINE_PI2;

  while (angle < 0.0f)
    angle += ENGINE_PI2;

  return angle;
}

// Vectors 2
// -------------------------------------------------------------------------

/**
 * @brief Constructs a vector
 */
#define engine_vec2_make(x, y) ((const engine_vec2_t){ x, y })

/**
 * @brief Returns true if the vectors are equal (within epsilon)
 */
ENGINE_INLINE bool
engine_vec2_equal(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_equal(v1.x, v2.x) && engine_equal(v1.y, v2.y);
}

/**
 * @brief Adds two vectors
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_add(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(v1.x + v2.x, v1.y + v2.y);
}

/**
 * @brief Subtracts two vectors
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_sub(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(v1.x - v2.x, v1.y - v2.y);
}

/**
 * @brief Multiplies two vectors component-wise
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_mul(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(v1.x * v2.x, v1.y * v2.y);
}

/**
 * @brief Divides two vectors component-wise
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_div(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(v1.x / v2.x, v1.y / v2.y);
}

/**
 * @brief Scales a vector
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_scale(engine_vec2_t v, float c)
{
  return engine_vec2_make(v.x * c, v.y * c);
}

/**
 * @brief Dot product
 */
ENGINE_INLINE float
engine_vec2_dot(engine_vec2_t v1, engine_vec2_t v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

/**
 * @brief Returns the square of the length of the vector
 */
ENGINE_INLINE float
engine_vec2_len2(engine_vec2_t v)
{
  return engine_vec2_dot(v, v);
}

/**
 * @brief Returns the length of the vector
 */
ENGINE_INLINE float
engine_vec2_len(engine_vec2_t v)
{
  return sqrt(engine_vec2_len2(v));
}

/**
 * @brief Normalizes a vector (sets its length to one)
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_normalize(engine_vec2_t v)
{
  float c = engine_vec2_len(v);

  if (c < ENGINE_EPSILON)
    return engine_vec2_make(0.0f, 0.0f);
  else
    return engine_vec2_scale(v, 1.0f / c);
}

/**
 * @brief Negates a vector (scales it by -1.0)
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_reflect(engine_vec2_t v)
{
  return engine_vec2_scale(v, -1.0f);
}

/**
 * @brief Construct a vector that is perpendicular to the specified vector
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_perp(engine_vec2_t v)
{
  return engine_vec2_make(-v.y, v.x);
}

/**
 * @brief A 2D analog of the 3D cross product
 */
ENGINE_INLINE float
engine_vec2_cross(engine_vec2_t v1, engine_vec2_t v2)
{
  engine_vec2_t perp = engine_vec2_perp(v1);
  return engine_vec2_dot(perp, v2);
}

/**
 * @brief Returns the angle the vector with respect to the current basis
 */
ENGINE_INLINE float
engine_vec2_angle(engine_vec2_t v)
{
  return atan2(v.y, v.x);
}

/**
 * @brief Projects a vector onto another
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_proj(engine_vec2_t v1, engine_vec2_t v2)
{
  float d = engine_vec2_dot(v1, v2) / engine_vec2_dot(v2, v2);

  return engine_vec2_scale(v2, d);
}

/**
 * @brief Returns the distance between the two vectors
 */
ENGINE_INLINE float
engine_vec2_dist(engine_vec2_t v1, engine_vec2_t v2)
{
  engine_vec2_t v = engine_vec2_sub(v1, v2);

  return engine_vec2_len(v);
}

/**
 * @brief Linearly interpolates between two vectors
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_lerp(engine_vec2_t v1, engine_vec2_t v2, float step)
{
  engine_vec2_t out;

  out.x = engine_lerp(v1.x, v2.x, step);
  out.y = engine_lerp(v1.y, v2.y, step);

  return out;
}

/**
 * @brief Returns the zero vector
 */
#define engine_vec2_t_zero() (engine_vec2_make(0.0f, 0.0f))

/**
 * @brief Contructs a vector in polar coordinates
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_polar(float angle, float len)
{
  return engine_vec2_make(len * cos(angle), len * sin(angle));
}

/**
 * @brief Computes the component-wise minimum of two vectors
 */
ENGINE_INLINE engine_vec2_t
vec2_min(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(engine_min(v1.x, v2.x), engine_min(v1.y, v2.y));
}

/**
 * @brief Computes the component-wise maximum of two vectors
 */
ENGINE_INLINE engine_vec2_t
vec2_max(engine_vec2_t v1, engine_vec2_t v2)
{
  return engine_vec2_make(engine_max(v1.x, v2.x), engine_max(v1.y, v2.y));
}

/**
 * @brief Computes the component-wise floor of the specified vector
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_floor(engine_vec2_t v)
{
  return engine_vec2_make(floor(v.x), floor(v.y));
}

/**
 * @brief Computes the component-wise ceiling of the specified vector
 */
ENGINE_INLINE engine_vec2_t
engine_vec2_ceil(engine_vec2_t v)
{
  return engine_vec2_make(ceil(v.x), ceil(v.y));
}

// Vectors 3
// -------------------------------------------------------------------------

/**
 * @brief Constructs a vector
 */
#define engine_vec3_make(x, y, z) ((const engine_vec3_t){ x, y, z })

/**
 * @brief Returns true if the vectors are equal (within epsilon)
 */
ENGINE_INLINE bool
engine_vec3_equal(engine_vec3_t v1, engine_vec3_t v2)
{
  return engine_equal(v1.x, v2.x) && engine_equal(v1.y, v2.y)
         && engine_equal(v1.z, v2.z);
}

/**
 * @brief Adds two vectors
 */
ENGINE_INLINE engine_vec3_t
engine_vec3_add(engine_vec3_t v1, engine_vec3_t v2)
{
  return engine_vec3_make(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

/**
 * @brief Subtracts two vectors
 */
ENGINE_INLINE engine_vec3_t
engine_vec3_sub(engine_vec3_t v1, engine_vec3_t v2)
{
  return engine_vec3_make(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

/**
 * @brief Multiplies two vectors component-wise
 */
ENGINE_INLINE engine_vec3_t
engine_vec3_mul(engine_vec3_t v1, engine_vec3_t v2)
{
  return engine_vec3_make(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

/**
 * @brief Divides two vectors component-wise
 */
ENGINE_INLINE engine_vec3_t
engine_vec3_div(engine_vec3_t v1, engine_vec3_t v2)
{
  return engine_vec3_make(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

// Rectangles
// -------------------------------------------------------------------------

/**
 * @brief Construct a rectangle
 */
#define engine_rect_make(x, y, w, h) ((const engine_rect_t){ x, y, w, h })

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

#endif // ENGINE_MATH_H_
