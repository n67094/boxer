<p align="center">
  <img src="logo.png" alt="Boxer logo" />
</p>

_The Boxer project draws its name from the boxer engine architecture found in cars, symbolizing efficiency in a compact form. The logo's left and right ends evoke the horizontally opposed pistons of the boxer engine's flat design._

# Boxer (Work in Progress)

A full-featured, lightweight and crossplatform 2D game library written in C.

Features:

- A File system for loading game assets from various archive formats like `.zip`, `.7Z` and way more.
- Image loading and rendering support for `.bmp` and `.png` formats.
- Read/write support for `.ini` and `.json` files.
- Keyboard, mouse, and gamepad input handling.
- Math utilities for vectors, matrices, and transformations.
- Font atlas utilities for efficient text rendering.
- A 2D stack painter for drawing sprites, shapes, and text with support for custom shaders.
- An entity-component system (ECS) for flexible game object management.

TODO:

- Immediate mode GUI system.
- Audio support.
- Physics utilities.
- and more.

## Prerequisites

Install the following development dependencies (via your package manager or from source):

- node (optional, for documentation generation).
- GNUMake.
- C compiler.
- SDL3 (>= 3.4.2).

## Build

Build and run the game example by cloning the repo:

```bash
git clone git@github.com:n67094/boxer.git
```

Debug build:

```bash
make DEBUG=1
```

Release build:

```bash
make
```

Run the example game:

```bash
./my_game
```

Build Docs:

```bash
make docs
```

Documentation files will be generated in the `docs/` directory as Markdown files.

_let's make it roar!_

# Sponsors

Hi everyone, I'm nsix, an indie game developer and open source contributor trying to make a living from my work.

If you enjoy what I create and want to support me, consider becoming a sponsor on [GitHub Sponsors](https://github.com/sponsors/n67094).
Every bit of support means a lot and is greatly appreciated!

You can also check out my blog at www.nsix.blog.

# License

This project is licensed under the `zlib` License. See the [LICENSE](LICENSE) file for details.
