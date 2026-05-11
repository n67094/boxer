<p align="center">
  <img src="logo.png" alt="Boxer logo" />
</p>

_The Boxer project draws its name from the boxer engine architecture found in cars, symbolizing efficiency in a compact form. The logo's left and right ends evoke the horizontally opposed pistons of the boxer engine's flat design._

> As long as I can remember, I've always wanted to make games. Somehow I ended up doing it, and a good way for me to enjoy this journey is to build my own game engine and share it with you, hoping it will help you along yours.

# Boxer (Work in Progress)

A minimal, full-featured, and high-performance 2D game engine written in C99, designed to let you easily build your game around it, favoring direct inclusion over linking as a static or shared library.

Features:

- a virtual file system to load game assets from `.zip`, `.7Z`, etc.
- `.bmp` and `.png` image loading and rendering support.
- `.ini` and `.json` configuration file reading and writing capabilities.
- Keyboard, mouse, and gamepad input handling.
- Math utilities for vectors, matrices, and transformations.
- Font atlas utilities for efficient text rendering.
- A powerful and efficient 2D painting system that allows custom graphic pipelines and shaders.
- An entity-component system (ECS) for flexible game object management.

TODO:

- File resource management system.
- Immediate mode GUI system.
- Audio support.
- Physics engine integration.
- and more.

## Prerequisites

Install the following development dependencies (via your package manager or from source):

- node (for documentation generation).
- GNUMake.
- C compiler.
- SDL3 (>= 3.4.2).

## Project Structure

The project is organized as follows:

```bash
data/  # Game assets such as textures, audio, etc.
src/   # Core engine source code.
game/  # A game example that demonstrates how to use the engine.
test/  # Unit tests for the engine.
```

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
