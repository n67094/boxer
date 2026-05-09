<a name="line-1"></a>
File `bxr_context.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Boxer context

TODO improve context API, add more functions to set/get context state (e.g.
window title, icon, etc.).

<a name="line-45"></a>
Get the global context instance.

`return` Pointer to the global context instance.

```cpp
bxr_context_t *bxr_context_get(void)
```

---

<a name="line-52"></a>
Check if the global context is valid (initialized).

`return` `true` if the context is valid, `false` otherwise.

```cpp
bool bxr_context_is_valid(void)
```

---

<a name="line-59"></a>
Set the window dimensions.

`width`  The new width of the window in pixels.
`height` The new height of the window in pixels.

```cpp
void bxr_context_set_window_dimensions(int width, int height)
```

---

<a name="line-67"></a>
Get the current window dimensions.

`return` The current window dimensions as a `bxr_vec2_t` (x = width, y =
height).

```cpp
bxr_vec2_t bxr_context_get_window_dimensions(void)
```

---

<a name="line-75"></a>
Set the fullscreen mode of the window.

`fullscreen` `true` to set the window to fullscreen mode, `false` to set it
to windowed mode.

```cpp
void bxr_context_set_fullscreen(bool fullscreen)
```

---

<a name="line-83"></a>
Check if the window is currently in fullscreen mode.

`return` `true` if the window is in fullscreen mode, `false` otherwise.

```cpp
bool bxr_context_is_fullscreen(void)
```

---

<a name="line-90"></a>
Set the resizable property of the window.

`resizable` `true` to make the window resizable, `false` to make it
non-resizable.

```cpp
void bxr_context_set_resizable(bool resizable)
```

---

<a name="line-98"></a>
Check if the window is currently resizable.

`return` `true` if the window is resizable, `false` otherwise.

```cpp
bool bxr_context_is_resizable(void)
```

---
