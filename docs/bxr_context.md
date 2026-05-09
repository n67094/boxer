<a name="line-1"></a>
File `bxr_context.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Boxer context

TODO improve context API, add more functions to set/get context state (e.g.
window title, icon, etc.).

<a name="line-45"></a>

```cpp
bxr_context_t *bxr_context_get(void)
```

Get the global context instance.

`return` Pointer to the global context instance.

---

<a name="line-52"></a>

```cpp
bool bxr_context_is_valid(void)
```

Check if the global context is valid (initialized).

`return` `true` if the context is valid, `false` otherwise.

---

<a name="line-59"></a>

```cpp
void bxr_context_set_window_dimensions(int width, int height)
```

Set the window dimensions.

`width`  The new width of the window in pixels.
`height` The new height of the window in pixels.

---

<a name="line-67"></a>

```cpp
bxr_vec2_t bxr_context_get_window_dimensions(void)
```

Get the current window dimensions.

`return` The current window dimensions as a `bxr_vec2_t` (x = width, y =
height).

---

<a name="line-75"></a>

```cpp
void bxr_context_set_fullscreen(bool fullscreen)
```

Set the fullscreen mode of the window.

`fullscreen` `true` to set the window to fullscreen mode, `false` to set it
to windowed mode.

---

<a name="line-83"></a>

```cpp
bool bxr_context_is_fullscreen(void)
```

Check if the window is currently in fullscreen mode.

`return` `true` if the window is in fullscreen mode, `false` otherwise.

---

<a name="line-90"></a>

```cpp
void bxr_context_set_resizable(bool resizable)
```

Set the resizable property of the window.

`resizable` `true` to make the window resizable, `false` to make it
non-resizable.

---

<a name="line-98"></a>

```cpp
bool bxr_context_is_resizable(void)
```

Check if the window is currently resizable.

`return` `true` if the window is resizable, `false` otherwise.

---
