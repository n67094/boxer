<a name="line-1"></a>
File `bxr_game.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Game lifecycle management

<a name="line-28"></a>
## 🔹 extern bxr_game_config_t *bxr_game_config(void)

```cpp
extern bxr_game_config_t *bxr_game_config(void)
```

The user should provide this function to return boxer's game configuration.

If a file named "config.ini" exist in the `write` mounted directory
boxer will try to read the configuration values from the file and override
the default configuration values returned by this function.

Here is the list of the configuration values that can be overridden by the
config.ini file:

- width
- height
- fullscreen
- resizable
- target_ups

Write mounted directories:

- **Linux:** `~/.local/share/<config->name>/`
- **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
- **macOS:** `~/Library/Application Support/<config->name>/`

`return` the game configuration to use for the game.

<a name="line-54"></a>
## 🔹 extern void bxr_game_setup(void)

```cpp
extern void bxr_game_setup(void)
```

The user should provide this function to setup the game.

<a name="line-59"></a>
## 🔹 extern void bxr_game_update(Uint64 delta_time_ms)

```cpp
extern void bxr_game_update(Uint64 delta_time_ms)
```

The user should provide this function to update the game logic.

`delta_time_ms` is the time in milliseconds since the last update call.

**NOTE:** The game is updated at a fixed rate defined by the `target_ups`
configuration value.

<a name="line-69"></a>
## 🔹 extern void bxr_game_render(Uint64 alpha_time_ms)

```cpp
extern void bxr_game_render(Uint64 alpha_time_ms)
```

The user should provide this function to render the game.

`alpha_time_ms` is the time in milliseconds since the last update call.

**NOTE:** The game is rendered at a variable rate, and `alpha_time_ms` can be
used to interpolate the game state for smooth rendering.

<a name="line-79"></a>
## 🔹 extern void bxr_game_shutdown(void)

```cpp
extern void bxr_game_shutdown(void)
```

The user should provide this function to shutdown the game.
