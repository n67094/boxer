<a name="line-1"></a>
File `bxr_mouse.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Mouse Input Handling

<a name="line-34"></a>
Internal API

<a name="line-38"></a>
Initialize the mouse subsystem. Must be called before using any other mouse
functions.

```cpp
void bxr_mouse_setup(void)
```

---

<a name="line-44"></a>
Update the mouse state for the current frame. Must be called at the beginning
of each frame before processing input.

```cpp
void bxr_mouse_begin_frame(void)
```

---

<a name="line-50"></a>
Shutdown the mouse subsystem. Must be called when the application is shutting
down.

```cpp
void bxr_mouse_shutdown(void)
```

---

<a name="line-56"></a>
Handles a mouse button down event. Must be called when a mouse button event
is received from SDL.

`button` is the mouse button that was pressed or released.

`click` is the type of click (single or double).

```cpp
void bxr_mouse_button_down(bxr_mouse_e button, bxr_mouse_click_e click)
```

---

<a name="line-66"></a>
Handles a mouse button up event. Must be called when a mouse button up event
is received from SDL.

`button` is the mouse button that was released.

```cpp
void bxr_mouse_button_up(bxr_mouse_e button)
```

---

<a name="line-74"></a>
Handles a mouse motion event. Must be called when a mouse motion event is
received from SDL.

`x` is the new x position of the mouse cursor.

`y` is the new y position of the mouse cursor.

```cpp
void bxr_mouse_motion(float x, float y)
```

---

<a name="line-84"></a>
Handles a mouse wheel motion event. Must be called when a mouse wheel motion
event is received from SDL.

`x` is the amount of horizontal scroll.

`y` is the amount of vertical scroll.

```cpp
void bxr_mouse_wheel_motion(float x, float y)
```

---

<a name="line-94"></a>
Public API

<a name="line-98"></a>
Get the current position of the mouse cursor.

`return` the current position of the mouse cursor.

```cpp
bxr_vec2_t bxr_mouse_pos(void)
```

---

<a name="line-105"></a>
Get the position of the mouse cursor in the previous frame.

`return` the position of the mouse cursor in the previous frame.

```cpp
bxr_vec2_t bxr_mouse_prev_pos(void)
```

---

<a name="line-112"></a>
Check if a mouse button is currently held down.

`button` is the mouse button to check.

`return` true if the button is currently held down, false otherwise.

```cpp
bool bxr_mouse_held(bxr_mouse_e button)
```

---

<a name="line-121"></a>
Check if a mouse button was just pressed this frame.

`button` is the mouse button to check.

`return` true if the button was just pressed this frame, false otherwise.

```cpp
bool bxr_mouse_just_pressed(bxr_mouse_e button)
```

---

<a name="line-130"></a>
Check if a mouse button was just released this frame.

`button` is the mouse button to check.

`return` true if the button was just released this frame, false otherwise.

```cpp
bool bxr_mouse_just_released(bxr_mouse_e button)
```

---

<a name="line-139"></a>
Check if a mouse button was double clicked this frame.

`button` is the mouse button to check.

`return` true if the button was double clicked this frame, false otherwise.

```cpp
bool bxr_mouse_double_clicked(bxr_mouse_e button)
```

---

<a name="line-148"></a>
Get the amount of time a mouse button has been held down.

`button` is the mouse button to check.

`return` the amount of time the button has been held down in milliseconds.

```cpp
Uint64 bxr_mouse_held_time(bxr_mouse_e button)
```

---

<a name="line-157"></a>
Get the amount of scroll from the mouse wheel in the current frame.

`return` the amount of scroll from the mouse wheel in the current frame.

```cpp
bxr_vec2_t bxr_mouse_wheel(void)
```

---

<a name="line-164"></a>
Hide or show the mouse cursor.

`hide` true to hide the mouse cursor, false to show it.

```cpp
void bxr_mouse_hide(bool hide)
```

---

<a name="line-171"></a>
Check if the mouse cursor is currently hidden.

`return` true if the mouse cursor is currently hidden, false otherwise.

```cpp
bool bxr_mouse_is_hidden(void)
```

---
