\mainpage Readme

<p align="center">
  <img src="logo.png" alt="Boxer engine logo" />
</p>

Boxer Engine
------------

A minimalist, modern, and lightweight 2D game engine written in C, designed for direct inclusion in your projects rather than linking as a static or shared library.

> "The Boxer project draws its name from the boxer engine architecture found in cars, symbolizing efficiency in a compact form."

_The logo's left and right ends evoke the horizontally opposed pistons of the boxer engine flat design._

Prerequisites
=============

  - Doxygen (optional, for documentation generation).
  - GNUMake.
  - C and C++ compiler.
  - SDL3 (install via your package manager).
  - SDL_shadercross (optional, only if you need custom shaders).
  - RenderDoc (optional, for debugging graphics).

Build and Run
=============

Clone the repo:

```bash
git clone git@github.com:Livy-s-Quest/boxer_engine.git
cd boxer-engine
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

let's make it roar!

Project Structure
=================

The project is organized as follows:

```bash
engine/ # Core engine source code
game/   # Your game source code
```

Project Filesystem
==================

Under the hood the engine use PhysFS for virtual filesystem and by default the following folders are mounted:

- `data.zip` is mounted as `/` (readonly) and is where the game should load assets from such as textures, audio, etc.
- `the system prefered path` is mounted as `/` (read/write) and is where the game should save user data such as save files, settings, etc.

This means that file are read from both `data.zip` and `the system prefered path` but are only written to `the system prefered path`. This allows the game to have a read-only assets folder and a read/write user data folder.

Depending on the operating system, the `system prefered path` is as follows:

- linux `~/.local/share/<config->name>/`
- windows `C:\Users\<username>\AppData\Roaming\<config->name>\`
- macos `~/Library/Application Support/<config->name>/`

License
=======

This project is licensed under the MIT License
