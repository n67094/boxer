<a name="line-1"></a>
File `bxr_math.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Mathematical and Geometric Utilities

<a name="line-84"></a>
## Scalar Functions and Macros

<a name="line-88"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

Computes the sign of the number.

`return`:

- -1 if `val` is less than zero;
- 0 if `val` is equal to zero;
- 1 if `val` is greater than zero.

<a name="line-103"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`return` `true` if the values are within epsilon of one another.

<a name="line-112"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

Linearly interpolates the two values.

`step` a number in [0, 1] that specifies the position between the values.

`return` the interpolated value.

<a name="line-125"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

Linearly interpolates between two angles.

`angle1` The first angle in radians.

`angle2` The second angle in radians.

`step` a number in [0, 1] that specifies the position between the
angles.

`return` the interpolated angle in radians.

<a name="line-150"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

Clamps the angle to be in [0, 2 * BXR_PI].

`angle` The angle in radians.

`return` the normalized angle in radians.

<a name="line-169"></a>
## Point

<a name="line-173"></a>
### 🔹 #define bxr_point_create(x, y) ((const bxr_point_t){ x, y })

```cpp
#define bxr_point_create(x, y) ((const bxr_point_t){ x, y })
```

Constructs a point.

<a name="line-178"></a>
Vectors 2

<a name="line-182"></a>
### 🔹 #define bxr_vec2_create(x, y) ((const bxr_vec2_t){ x, y })

```cpp
#define bxr_vec2_create(x, y) ((const bxr_vec2_t){ x, y })
```

Constructs a vector.

<a name="line-187"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`returns` true if the vectors are equal (within epsilon).

<a name="line-196"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`retrurn` the sum of two vectors.

<a name="line-205"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the difference of two vectors.

<a name="line-214"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```


`return` the product of two vectors.

<a name="line-224"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the quotient of two vectors.

<a name="line-233"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the vector scaled by the specified scalar.

<a name="line-242"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` the dot product of two vectors.

<a name="line-251"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` the squared length of the vector.

<a name="line-260"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` the length of the vector (magnitude).

<a name="line-269"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the normalized vector (vector with length 1). If the vector has zero
length, the zero vector is returned.

<a name="line-284"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the negation of the vector.

<a name="line-293"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` a vector that is perpendicular to the specified vector.

<a name="line-302"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` A 2D analog of the 3D cross product.

<a name="line-312"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` the angle the vector with respect to the current basis.

<a name="line-321"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the projection of `v1` onto `v2`.

<a name="line-332"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`return` the distance between the two vectors.

<a name="line-343"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the linearly interpolates between two vectors.

<a name="line-357"></a>
### 🔹 #define bxr_vec2_t_zero() (bxr_vec2_create(0.0f, 0.0f))

```cpp
#define bxr_vec2_t_zero() (bxr_vec2_create(0.0f, 0.0f))
```

`returns` the zero vector.

<a name="line-362"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` a vector in polar coordinates.

<a name="line-371"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the component-wise minimum of two vectors.

<a name="line-380"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the component-wise maximum of two vectors.

<a name="line-389"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the component-wise floor of the specified vector.

<a name="line-398"></a>
### 🔧 BXR_INLINE bxr_vec2_t

```cpp
BXR_INLINE bxr_vec2_t
```

`return` the component-wise ceiling of the specified vector.

<a name="line-407"></a>
## Line

<a name="line-417"></a>
### 🔹 #define bxr_line_create(ax, ay, bx, by)                                        \

```cpp
#define bxr_line_create(ax, ay, bx, by)                                        \
```

Construct a line

<a name="line-423"></a>
## Triangle

<a name="line-434"></a>
### 🔹 #define bxr_triangle_create(ax, ay, bx, by, cx, cy)                            \

```cpp
#define bxr_triangle_create(ax, ay, bx, by, cx, cy)                            \
```

Construct a triangle

<a name="line-440"></a>
## Rectangle

<a name="line-449"></a>
### 🔹 #define bxr_rect_create(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })

```cpp
#define bxr_rect_create(x, y, w, h) ((const bxr_rect_t){ x, y, w, h })
```

Construct a rectangle

<a name="line-454"></a>
### 🔧 BXR_INLINE float

```cpp
BXR_INLINE float
```

`returns` the area of the rectangle

<a name="line-463"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`returns` true if the rectangles overlap

<a name="line-481"></a>
### 🔧 BXR_INLINE bxr_rect_t

```cpp
BXR_INLINE bxr_rect_t
```

`returns` the intersection of the two rectangles.

If they don't overlap, `return` a rectangle starting at r2 x, y with zero
width and height.

<a name="line-504"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`return` true if r1 completely contains r2.

<a name="line-522"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`return` true if the rectangle contains the point.

<a name="line-540"></a>
### 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

`return` true if the rectangles are adjacent (share a side).

<a name="line-552"></a>
### 🔧 BXR_INLINE bxr_rect_t

```cpp
BXR_INLINE bxr_rect_t
```

`return` a rectangle that is expanded by the specified amount in all
directions.

<a name="line-565"></a>
### 🔧 bxr_rect_t BXR_INLINE

```cpp
bxr_rect_t BXR_INLINE
```

`return` a rectangle that is contracted by the specified amount in all
directions.

<a name="line-578"></a>
## Textured Rectangle

<a name="line-588"></a>
### 🔹 #define bxr_textured_rect_create(dst, src)                                     \

```cpp
#define bxr_textured_rect_create(dst, src)                                     \
```

Construct a textured rectangle.

<a name="line-594"></a>
## Region

<a name="line-603"></a>
### 🔹 #define bxr_region_create(x0, y0, x1, y1)                                      \

```cpp
#define bxr_region_create(x0, y0, x1, y1)                                      \
```

Construct a region
