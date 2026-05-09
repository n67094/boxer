<a name="line-1"></a>
File `bxr_math.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Mathematical and Geometric Utilities

<a name="line-84"></a>
## Scalar Functions and Macros

<a name="line-88"></a>
Computes the sign of the number.

`return`:

- -1 if `val` is less than zero;
- 0 if `val` is equal to zero;
- 1 if `val` is greater than zero.

```cpp
BXR_INLINE float
```

---

<a name="line-103"></a>
`return` `true` if the values are within epsilon of one another.

```cpp
BXR_INLINE bool
```

---

<a name="line-112"></a>
Linearly interpolates the two values.

`step` a number in [0, 1] that specifies the position between the values.

`return` the interpolated value.

```cpp
BXR_INLINE float
```

---

<a name="line-125"></a>
Linearly interpolates between two angles.

`angle1` The first angle in radians.

`angle2` The second angle in radians.

`step` a number in [0, 1] that specifies the position between the
angles.

`return` the interpolated angle in radians.

```cpp
BXR_INLINE float
```

---

<a name="line-150"></a>
Clamps the angle to be in [0, 2 * BXR_PI].

`angle` The angle in radians.

`return` the normalized angle in radians.

```cpp
BXR_INLINE float
```

---

<a name="line-169"></a>
## Point

<a name="line-173"></a>
Constructs a point.

```cpp
#define bxr_point_create(x, y) ((const bxr_point_t){ x, y })
```

---

<a name="line-178"></a>
Vectors 2

<a name="line-182"></a>
Constructs a vector.

```cpp
#define bxr_vec2_create(x, y) ((const bxr_vec2_t){ x, y })
```

---

<a name="line-187"></a>
`returns` true if the vectors are equal (within epsilon).

```cpp
BXR_INLINE bool
```

---

<a name="line-196"></a>
`retrurn` the sum of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-205"></a>
`return` the difference of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-214"></a>

`return` the product of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-224"></a>
`return` the quotient of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-233"></a>
`return` the vector scaled by the specified scalar.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-242"></a>
`return` the dot product of two vectors.

```cpp
BXR_INLINE float
```

---

<a name="line-251"></a>
`return` the squared length of the vector.

```cpp
BXR_INLINE float
```

---

<a name="line-260"></a>
`return` the length of the vector (magnitude).

```cpp
BXR_INLINE float
```

---

<a name="line-269"></a>
`return` the normalized vector (vector with length 1). If the vector has zero
length, the zero vector is returned.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-284"></a>
`return` the negation of the vector.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-293"></a>
`return` a vector that is perpendicular to the specified vector.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-302"></a>
`return` A 2D analog of the 3D cross product.

```cpp
BXR_INLINE float
```

---

<a name="line-312"></a>
`return` the angle the vector with respect to the current basis.

```cpp
BXR_INLINE float
```

---

<a name="line-321"></a>
`return` the projection of `v1` onto `v2`.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-332"></a>
`return` the distance between the two vectors.

```cpp
BXR_INLINE float
```

---

<a name="line-343"></a>
`return` the linearly interpolates between two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-357"></a>
`returns` the zero vector.

```cpp
#define bxr_vec2_t_zero() (bxr_vec2_create(0.0f, 0.0f))
```

---

<a name="line-362"></a>
`return` a vector in polar coordinates.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-371"></a>
`return` the component-wise minimum of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-380"></a>
`return` the component-wise maximum of two vectors.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-389"></a>
`return` the component-wise floor of the specified vector.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-398"></a>
`return` the component-wise ceiling of the specified vector.

```cpp
BXR_INLINE bxr_vec2_t
```

---

<a name="line-407"></a>
## Line

<a name="line-417"></a>
Construct a line

```cpp
#define bxr_line_create(ax, ay, bx, by)                                        \
```

---

<a name="line-423"></a>
## Triangle

<a name="line-434"></a>
Construct a triangle

```cpp
#define bxr_triangle_create(ax, ay, bx, by, cx, cy)                            \
```

---

<a name="line-440"></a>
## Rectangle

<a name="line-449"></a>
Construct a rectangle

```cpp
#define bxr_rect_create(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })
```

---

<a name="line-454"></a>
`returns` the area of the rectangle

```cpp
BXR_INLINE float
```

---

<a name="line-463"></a>
`returns` true if the rectangles overlap

```cpp
BXR_INLINE bool
```

---

<a name="line-481"></a>
`returns` the intersection of the two rectangles.

If they don't overlap, `return` a rectangle starting at r2 x, y with zero
width and height.

```cpp
BXR_INLINE bxr_rect_t
```

---

<a name="line-504"></a>
`return` true if r1 completely contains r2.

```cpp
BXR_INLINE bool
```

---

<a name="line-522"></a>
`return` true if the rectangle contains the point.

```cpp
BXR_INLINE bool
```

---

<a name="line-540"></a>
`return` true if the rectangles are adjacent (share a side).

```cpp
BXR_INLINE bool
```

---

<a name="line-552"></a>
`return` a rectangle that is expanded by the specified amount in all
directions.

```cpp
BXR_INLINE bxr_rect_t
```

---

<a name="line-565"></a>
`return` a rectangle that is contracted by the specified amount in all
directions.

```cpp
bxr_rect_t BXR_INLINE
```

---

<a name="line-578"></a>
## Textured Rectangle

<a name="line-588"></a>
Construct a textured rectangle.

```cpp
#define bxr_textured_rect_create(dst, src)                                     \
```

---

<a name="line-594"></a>
## Region

<a name="line-603"></a>
Construct a region

```cpp
#define bxr_region_create(x0, y0, x1, y1)                                      \
```

---
