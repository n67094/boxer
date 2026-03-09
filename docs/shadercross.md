Shadercross
---------------

Shader cross-compiler utility for SDL applications.

**Link:** [https://github.com/libsdl-org/SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross)

Shadercross Setup (Linux)
-----------------------------

To install the `SPIRV-Cross` dependencies:

```bash
git clone https://github.com/KhronosGroup/SPIRV-Cross
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSPIRV_CROSS_SHARED=ON
cmake --build build --parallel
sudo cmake --install build
```

To install the `DirectXShaderCompile` dependencies (https://github.com/microsoft/DirectXShaderCompiler/releases)[here], download the latest linux version extra it into a folder you wont delete then:

```bash
sudo ln -sf bin /usr/local/bin/
sudo ln -sf include /usr/local/include/
sudo ln -sf lib /usr/local/lib/
sudo ldconfig
```

Finally to install `SDL_shadercross` with:

```bash
$ git clone https://github.com/libsdl-org/SDL_shadercross
$ cd SDL_shadercross
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --parallel
$ sudo cmake --install build
```

You can now use `shadercross`.

How to Compile a shader
-----------------------

Install glslang using your package manager, then compile a GLSL shader to SPIR-V using those commands:

```bash
glslangValidator -V shader.frag.glsl -o shader.frag.spv
glslangValidator -V shader.vert.glsl -o shader.vert.spv
```

Then convert the SPIR-V shader to other formats as follow:

```bash
shadercross shader.vert.spv -o shader.frag.dxil
shadercross shader.frag.spv -o shader.frag.dxil
shadercross shader.vert.spv -o shader.frag.msl
shadercross shader.frag.spv -o shader.frag.msl
```

How to Setup RenderDoc
----------------------

**NOTE:** `renderdoc` doesn't support `Wayland` yet, so make sure you are running an X11 session (c.f https://wiki.libsdl.org/SDL3/FAQDevelopment#renderdoc).

To debug and inspect your shaders, you can use RenderDoc by Downloading it from (https://renderdoc.org/)[https://renderdoc.org/]:

```bash
cd linux_dxc_2025_07_14.x86_64 # The downloaded file
sudo ln -sf bin /usr/local/bin/
sudo ln -sf etc /usr/local/etc/
sudo ln -sf include /usr/local/include/
sudo ln -sf lib /usr/local/lib/
sudo ln -sf share /usr/local/share/
sudo ldconfig
```

Test it by running:

```bash
qrenderdoc
```
