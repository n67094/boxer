<a name="line-1"></a>
File `bxr_gamepad.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Gamepad Input Handling

<a name="line-61"></a>
## Internal API

<a name="line-65"></a>
Initializes the gamepad subsystem. Must be called before using any other
gamepad functions.

```cpp
void bxr_gamepad_setup(void)
```

---

<a name="line-71"></a>
Shuts down the gamepad subsystem. Must be called when the application is
shutting down.

```cpp
void bxr_gamepad_shutdown(void)
```

---

<a name="line-77"></a>
Listens for gamepad events and updates the internal state of the gamepads.
Must be called every frame to ensure that the gamepad state is updated.

```cpp
void bxr_gamepad_listen(void)
```

---

<a name="line-83"></a>
Update the gamepad state for the current frame. Must be called at the
beginning of each frame before processing input.

```cpp
void bxr_gamepad_begin_frame(void)
```

---

<a name="line-89"></a>
Handles a gamepad button down event. Must be called when a gamepad button
down event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`button` is the button that was pressed.

```cpp
void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button)
```

---

<a name="line-99"></a>
Handles a gamepad button up event. Must be called when a gamepad button up
event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`button` is the button that was released.

```cpp
void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button)
```

---

<a name="line-109"></a>
Handles a gamepad axis motion event. Must be called when a gamepad axis
motion event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`axis` is the axis that was moved.

`value` is the new value of the axis, normalized to the range [-1, 1].

```cpp
void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value)
```

---

<a name="line-121"></a>
## Public API

<a name="line-125"></a>
Get the number of connected gamepads.

`return` the number of connected gamepads.

```cpp
int bxr_gamepad_count(void)
```

---

<a name="line-132"></a>
Set the deadzone for gamepad axes.

`deadzone` is the deadzone to set.

```cpp
void bxr_gamepad_set_deadzone(float deadzone)
```

---

<a name="line-139"></a>
Get the current deadzone for gamepad axes.

`return` the current deadzone for gamepad axes.

```cpp
float bxr_gamepad_get_deadzone(void)
```

---

<a name="line-146"></a>
Check if a gamepad is connected.

`index` is the index of the gamepad to check.

`return` true if the gamepad is connected, false otherwise.

```cpp
bool bxr_gamepad_connected(int index)
```

---

<a name="line-155"></a>
Get the name of a gamepad.

`index` is the index of the gamepad to get the name of.

`return` the name of the gamepad, or NULL if the gamepad is not connected.

```cpp
const char *bxr_gamepad_name(int index)
```

---

<a name="line-164"></a>
Get the serial number of a gamepad.

`index` is the index of the gamepad to get the serial number of.

`return` the serial number of the gamepad, or NULL if the gamepad is not
connected.

```cpp
const char *bxr_gamepad_serial(int index)
```

---

<a name="line-174"></a>
Check if a gamepad button is currently held down.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button is currently held down, false otherwise.

```cpp
bool bxr_gamepad_held(int index, bxr_gamepad_button_e button)
```

---

<a name="line-185"></a>
Check if a gamepad button was just pressed this frame.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button was just pressed this frame, false otherwise.

```cpp
bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button)
```

---

<a name="line-196"></a>
Check if a gamepad button was just released this frame.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button was just released this frame, false otherwise.

```cpp
bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button)
```

---

<a name="line-207"></a>
Get the amount of time a gamepad button has been held down.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` the amount of time the button has been held down in milliseconds.

```cpp
Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button)
```

---

<a name="line-218"></a>
Get the current value of a gamepad axis.

`index` is the index of the gamepad to check.

`axis` is the axis to check.

`return` the current value of the axis.

```cpp
float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis)
```

---

<a name="line-229"></a>
Get the value of a gamepad axis in the previous frame.

`index` is the index of the gamepad to check.

`axis` is the axis to check.

`return` the value of the axis in the previous frame.

```cpp
float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis)
```

---

<a name="line-240"></a>
Set the rumble state of a gamepad.

`index` is the index of the gamepad to set the rumble state of.

`low_frequency` is the intensity of the low frequency rumble motor.

`high_frequency` is the intensity of the high frequency rumble motor.

`duration_ms` is the duration of the rumble effect in milliseconds.

`return` true if the rumble state was successfully set, false otherwise.

```cpp
bool bxr_gamepad_rumble(int index,
```

---
