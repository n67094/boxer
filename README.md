<p align="center">
  <img src="logo.png" alt="Boxer logo" />
</p>

_The logo's left and right ends evoke the horizontally opposed pistons of the boxer engine flat design._

> As long as I can remember, I've always wanted to make games. Somehow I ended up doing it, and a good way for me to enjoy this journey is to build my own game engine and share it with you, hoping it will help you too.

# Boxer (Work in Progress)

_"The Boxer project draws its name from the boxer engine architecture found in cars, symbolizing efficiency in a compact form."_

A minimal and high-performance 2D game engine written in C99, designed so you can easily build your game around it, favoring direct inclusion rather than linking as a static or shared library.

## Prerequisites

Install the following development dependencies (via your package manager or from source):

- GNUMake.
- C compiler.
- SDL3 (>= 3.4.2).

## Build and Run

Clone the repo:

```bash
git clone git@github.com:Livy-s-Quest/boxer.git
cd boxer
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

# Project Structure

The project is organized as follows:

```bash
data/  # Game assets such as textures, audio, etc.
src/ # Core engine source code
game/  # Your game source code
test/  # Unit tests for the engine (take a look to see examples on how to use the engine)
```

# Sponsors

Hi everyone, I'm nsix, an indie game developer and open source contributor trying to make a living from my work.

If you enjoy what I create and want to support me, consider becoming a sponsor on [GitHub Sponsors](https://github.com/sponsors/n67094).
Every bit of support means a lot and is greatly appreciated!

You can also check out my blog at www.nsix.blog.

# License

This project is licensed under the MIT License
