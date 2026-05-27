<p align="center">
  <img src="logo.png" alt="Boxer logo" />
</p>

_The Boxer project draws its name from the boxer engine architecture found in cars, symbolizing efficiency in a compact form. The logo's left and right ends evoke the horizontally opposed pistons of the boxer engine's flat design._

# Boxer (Work in Progress)

A full-featured, lightweight and cross-platform 2D game library written in C.

Features:

- A file system for loading game assets from various archive formats like `.zip`, `.7z` and way more.
- Image loading and rendering support for `.bmp` and `.png` formats.
- Read/write support for `.ini` and `.json` files.
- Keyboard, mouse, and gamepad input handling.
- Math utilities for vectors, matrices, and transformations.
- A 2D stack painter for drawing sprites, shapes, and text with support for custom shaders.
- An entity-component system (ECS) for flexible game object management.

TODO:

- Immediate mode GUI system
- Audio support
- Physics utilities
- coroutine support
- Networking utilities
- and more

## Prerequisites

Install the following development dependencies (via your package manager or from source):

- node (optional, for documentation generation).
- GNU Make.
- C compiler.
- SDL3 (>= 3.4.2).

## Build and Installation

First clone the repository with:

```bash
git clone git@github.com:n67094/boxer.git
git submodule update --init --recursive
```

Build the library with:

```bash
make
```

Install the library at `/usr/lib` by default:

```bash
make install
```

Uninstall the library with:

```bash
make uninstall
```

Build documentation with:

```bash
make docs
```

Once `boxer` is installed on your system, in order to run the example you may need to update the shared library cache with:

```bash
sudo ldconfig
```

Then you can build the example with:

```bash
cd example
make
./example
```

_let's make it roar!_

# Getting Started

To get started with Boxer, you can:

- Check out the [example](example) directory for code demonstrating how to use the library's features.
- Generate the documentation with `make docs` to get detailed information on the API and how to integrate use it in your projects.

Here is a simple example of how to initialize the library and create a window:

```c
#include "bxr.h"

// Define the game configuration
static bxr_game_config_t _config = { 0 };

// Allows boxer to get the game configuration
bxr_game_config_t *
bxr_game_config(void)
{
  _config.name  = "application name";
  _config.title = "Window Title";

  _config.width  = 1280;
  _config.height = 720;

  _config.fullscreen = false;
  _config.resizable  = true;

  return &_config;
}

void
bxr_game_setup(void)
{
    // Initialize game state here
}

void
bxr_game_update(Uint64 delta_time_ms)
{
    // Update the game logic here.
    // The logic is updated at a fixed timestep depending on the screen refresh rate it's running on
}

void
bxr_game_render(Uint64 alpha_time_ms)
{
    // Render the game here.
    // The game is rendered as many times per second as possible
}

void
bxr_game_teardown(void)
{
    // Clean up game state here
}
```

# Sponsors

Hi everyone, I'm nsix, an indie game developer and open source contributor trying to make a living from my work.

If you enjoy what I create and want to support me, consider becoming a sponsor on [GitHub Sponsors](https://github.com/sponsors/n67094).
Every bit of support means a lot and is greatly appreciated!

You can also check out my blog at www.nsix.blog.

# License

This project is licensed under the `zlib` License. See the [LICENSE](LICENSE) file for details.
