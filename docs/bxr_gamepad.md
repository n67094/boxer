<a name="line-1"></a>
File `bxr_gamepad.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Gamepad Input Handling

<a name="line-61"></a>
## Internal API

<a name="line-65"></a>
### 🔹 void bxr_gamepad_setup(void)

```cpp
void bxr_gamepad_setup(void)
```

Initializes the gamepad subsystem. Must be called before using any other
gamepad functions.

<a name="line-71"></a>
### 🔹 void bxr_gamepad_shutdown(void)

```cpp
void bxr_gamepad_shutdown(void)
```

Shuts down the gamepad subsystem. Must be called when the application is
shutting down.

<a name="line-77"></a>
### 🔹 void bxr_gamepad_listen(void)

```cpp
void bxr_gamepad_listen(void)
```

Listens for gamepad events and updates the internal state of the gamepads.
Must be called every frame to ensure that the gamepad state is updated.

<a name="line-83"></a>
### 🔹 void bxr_gamepad_begin_frame(void)

```cpp
void bxr_gamepad_begin_frame(void)
```

Update the gamepad state for the current frame. Must be called at the
beginning of each frame before processing input.

<a name="line-89"></a>
### 🔹 void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button)

```cpp
void bxr_gamepad_button_down(int index, bxr_gamepad_button_e button)
```

Handles a gamepad button down event. Must be called when a gamepad button
down event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`button` is the button that was pressed.

<a name="line-99"></a>
### 🔹 void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button)

```cpp
void bxr_gamepad_button_up(int index, bxr_gamepad_button_e button)
```

Handles a gamepad button up event. Must be called when a gamepad button up
event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`button` is the button that was released.

<a name="line-109"></a>
### 🔹 void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value)

```cpp
void bxr_gamepad_axis_motion(int index, bxr_gamepad_axis_e axis, float value)
```

Handles a gamepad axis motion event. Must be called when a gamepad axis
motion event is received from SDL.

`index` is the index of the gamepad that the event occurred on.

`axis` is the axis that was moved.

`value` is the new value of the axis, normalized to the range [-1, 1].

<a name="line-121"></a>
## Public API

<a name="line-125"></a>
### 🔹 int bxr_gamepad_count(void)

```cpp
int bxr_gamepad_count(void)
```

Get the number of connected gamepads.

`return` the number of connected gamepads.

<a name="line-132"></a>
### 🔹 void bxr_gamepad_set_deadzone(float deadzone)

```cpp
void bxr_gamepad_set_deadzone(float deadzone)
```

Set the deadzone for gamepad axes.

`deadzone` is the deadzone to set.

<a name="line-139"></a>
### 🔹 float bxr_gamepad_get_deadzone(void)

```cpp
float bxr_gamepad_get_deadzone(void)
```

Get the current deadzone for gamepad axes.

`return` the current deadzone for gamepad axes.

<a name="line-146"></a>
### 🔹 bool bxr_gamepad_connected(int index)

```cpp
bool bxr_gamepad_connected(int index)
```

Check if a gamepad is connected.

`index` is the index of the gamepad to check.

`return` true if the gamepad is connected, false otherwise.

<a name="line-155"></a>
### 🔹 const char *bxr_gamepad_name(int index)

```cpp
const char *bxr_gamepad_name(int index)
```

Get the name of a gamepad.

`index` is the index of the gamepad to get the name of.

`return` the name of the gamepad, or NULL if the gamepad is not connected.

<a name="line-164"></a>
### 🔹 const char *bxr_gamepad_serial(int index)

```cpp
const char *bxr_gamepad_serial(int index)
```

Get the serial number of a gamepad.

`index` is the index of the gamepad to get the serial number of.

`return` the serial number of the gamepad, or NULL if the gamepad is not
connected.

<a name="line-174"></a>
### 🔹 bool bxr_gamepad_held(int index, bxr_gamepad_button_e button)

```cpp
bool bxr_gamepad_held(int index, bxr_gamepad_button_e button)
```

Check if a gamepad button is currently held down.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button is currently held down, false otherwise.

<a name="line-185"></a>
### 🔹 bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button)

```cpp
bool bxr_gamepad_just_pressed(int index, bxr_gamepad_button_e button)
```

Check if a gamepad button was just pressed this frame.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button was just pressed this frame, false otherwise.

<a name="line-196"></a>
### 🔹 bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button)

```cpp
bool bxr_gamepad_just_released(int index, bxr_gamepad_button_e button)
```

Check if a gamepad button was just released this frame.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` true if the button was just released this frame, false otherwise.

<a name="line-207"></a>
### 🔹 Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button)

```cpp
Uint64 bxr_gamepad_held_time(int index, bxr_gamepad_button_e button)
```

Get the amount of time a gamepad button has been held down.

`index` is the index of the gamepad to check.

`button` is the button to check.

`return` the amount of time the button has been held down in milliseconds.

<a name="line-218"></a>
### 🔹 float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis)

```cpp
float bxr_gamepad_axis(int index, bxr_gamepad_axis_e axis)
```

Get the current value of a gamepad axis.

`index` is the index of the gamepad to check.

`axis` is the axis to check.

`return` the current value of the axis.

<a name="line-229"></a>
### 🔹 float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis)

```cpp
float bxr_gamepad_axis_prev(int index, bxr_gamepad_axis_e axis)
```

Get the value of a gamepad axis in the previous frame.

`index` is the index of the gamepad to check.

`axis` is the axis to check.

`return` the value of the axis in the previous frame.

<a name="line-240"></a>
### 🔹 bool bxr_gamepad_rumble(int index,

```cpp
bool bxr_gamepad_rumble(int index,
```

Set the rumble state of a gamepad.

`index` is the index of the gamepad to set the rumble state of.

`low_frequency` is the intensity of the low frequency rumble motor.

`high_frequency` is the intensity of the high frequency rumble motor.

`duration_ms` is the duration of the rumble effect in milliseconds.

`return` true if the rumble state was successfully set, false otherwise.
