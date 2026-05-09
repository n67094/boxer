<a name="line-1"></a>
File `bxr_image.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Image Management and Utilities

<a name="line-20"></a>
## Internal API

<a name="line-24"></a>
Initialize the image subsystem. Must be called before using any other image
functions.

`context` is the bxr context to use for image operations.

```cpp
void bxr_image_setup(bxr_context_t *context)
```

---

<a name="line-32"></a>
Shutdown the image subsystem.

```cpp
void bxr_image_shutdown(void)
```

---

<a name="line-37"></a>
## Public API

<a name="line-41"></a>
Load an image from a file.

`path` is the path to the image file to load, relative to the data directory.

`return` the loaded image, or an invalid image if the image could not be
loaded. Use `bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned image using
`bxr_image_destroy` when it is no longer needed.

```cpp
bxr_image_t bxr_image_create(const char *path)
```

---

<a name="line-54"></a>
Create an image from memory.

`width` is the width of the image in pixels.

`height` is the height of the image in pixels.

`pixels` is a pointer to the pixel data in memory.

`return` the created image, or an invalid image if the image could not be
created. Use `bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned image using
`bxr_image_destroy` when it is no longer needed.

```cpp
bxr_image_t
```

---

<a name="line-72"></a>
Get the width of an image in pixels.

`image` is the image to get the width of.

`return` the width of the image in pixels, or 0 if the image is invalid.

```cpp
int bxr_image_get_width(bxr_image_t image)
```

---

<a name="line-81"></a>
Get the height of an image in pixels.

`image` is the image to get the height of.

`return` the height of the image in pixels, or 0 if the image is invalid.

```cpp
int bxr_image_get_height(bxr_image_t image)
```

---

<a name="line-90"></a>
Destroy an image and free its resources.

`image` is the image to destroy.

```cpp
void bxr_image_destroy(bxr_image_t image)
```

---
