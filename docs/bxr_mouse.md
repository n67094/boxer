<a name="line-1"></a>
File `bxr_mouse.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Mouse Input Handling

<a name="line-34"></a>
Internal API

<a name="line-38"></a>

```cpp
void bxr_mouse_setup(void)
```

Initialize the mouse subsystem. Must be called before using any other mouse
functions.

---

<a name="line-44"></a>

```cpp
void bxr_mouse_begin_frame(void)
```

Update the mouse state for the current frame. Must be called at the beginning
of each frame before processing input.

---

<a name="line-50"></a>

```cpp
void bxr_mouse_shutdown(void)
```

Shutdown the mouse subsystem. Must be called when the application is shutting
down.

---

<a name="line-56"></a>

```cpp
void bxr_mouse_button_down(bxr_mouse_e button, bxr_mouse_click_e click)
```

Handles a mouse button down event. Must be called when a mouse button event
is received from SDL.

`button` is the mouse button that was pressed or released.

`click` is the type of click (single or double).

---

<a name="line-66"></a>

```cpp
void bxr_mouse_button_up(bxr_mouse_e button)
```

Handles a mouse button up event. Must be called when a mouse button up event
is received from SDL.

`button` is the mouse button that was released.

---

<a name="line-74"></a>

```cpp
void bxr_mouse_motion(float x, float y)
```

Handles a mouse motion event. Must be called when a mouse motion event is
received from SDL.

`x` is the new x position of the mouse cursor.

`y` is the new y position of the mouse cursor.

---

<a name="line-84"></a>

```cpp
void bxr_mouse_wheel_motion(float x, float y)
```

Handles a mouse wheel motion event. Must be called when a mouse wheel motion
event is received from SDL.

`x` is the amount of horizontal scroll.

`y` is the amount of vertical scroll.

---

<a name="line-94"></a>
Public API

<a name="line-98"></a>

```cpp
bxr_vec2_t bxr_mouse_pos(void)
```

Get the current position of the mouse cursor.

`return` the current position of the mouse cursor.

---

<a name="line-105"></a>

```cpp
bxr_vec2_t bxr_mouse_prev_pos(void)
```

Get the position of the mouse cursor in the previous frame.

`return` the position of the mouse cursor in the previous frame.

---

<a name="line-112"></a>

```cpp
bool bxr_mouse_held(bxr_mouse_e button)
```

Check if a mouse button is currently held down.

`button` is the mouse button to check.

`return` true if the button is currently held down, false otherwise.

---

<a name="line-121"></a>

```cpp
bool bxr_mouse_just_pressed(bxr_mouse_e button)
```

Check if a mouse button was just pressed this frame.

`button` is the mouse button to check.

`return` true if the button was just pressed this frame, false otherwise.

---

<a name="line-130"></a>

```cpp
bool bxr_mouse_just_released(bxr_mouse_e button)
```

Check if a mouse button was just released this frame.

`button` is the mouse button to check.

`return` true if the button was just released this frame, false otherwise.

---

<a name="line-139"></a>

```cpp
bool bxr_mouse_double_clicked(bxr_mouse_e button)
```

Check if a mouse button was double clicked this frame.

`button` is the mouse button to check.

`return` true if the button was double clicked this frame, false otherwise.

---

<a name="line-148"></a>

```cpp
Uint64 bxr_mouse_held_time(bxr_mouse_e button)
```

Get the amount of time a mouse button has been held down.

`button` is the mouse button to check.

`return` the amount of time the button has been held down in milliseconds.

---

<a name="line-157"></a>

```cpp
bxr_vec2_t bxr_mouse_wheel(void)
```

Get the amount of scroll from the mouse wheel in the current frame.

`return` the amount of scroll from the mouse wheel in the current frame.

---

<a name="line-164"></a>

```cpp
void bxr_mouse_hide(bool hide)
```

Hide or show the mouse cursor.

`hide` true to hide the mouse cursor, false to show it.

---

<a name="line-171"></a>

```cpp
bool bxr_mouse_is_hidden(void)
```

Check if the mouse cursor is currently hidden.

`return` true if the mouse cursor is currently hidden, false otherwise.

---
