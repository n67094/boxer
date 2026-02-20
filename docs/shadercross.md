Shadercross
---------------

Shader cross-compiler utility for SDL applications.

**Link:** [https://github.com/libsdl-org/SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross)

---

Shadercross Setup (Linux)
-----------------------------

A quick guide to set up a Linux environment for GPU shader programming with SDL_gpu and SDL_shadercross.

Install dependencies, Starting with `SPIRV-Cross`:

```bash
$ git clone https://github.com/KhronosGroup/SPIRV-Cross
$ cd SPIRV-Cross
$ cmake --build build --target install
$ sudo cmake --build build --target install
```

Then with (https://github.com/microsoft/DirectXShaderCompiler/releases)[https://github.com/microsoft/DirectXShaderCompiler/releases], download the latest linux release then:

```bash
$ cd linux_dxc_2025_07_14.x86_64 # The downloaded file
$ sudo cp -r bin/* /usr/local/bin/
$ sudo cp -r include/* /usr/local/include/
$ sudo cp -r lib/* /usr/local/lib/
$ sudo ldconfig
```

Now clone SDL_shadercross and build it:

```bash
$ git clone https://github.com/libsdl-org/SDL_shadercross
$ cd SDL_shadercross
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo cp shadercross /usr/local/bin/
$ sudo ldconfig
```

You can now use `shadercross` from anywhere. Try `which shadercross` to confirm.

---

How to Compile a shader
-----------------------

Install glslang if you don't have it yet:

```bash
$ sudo dnf install glslang
```

Then compile a GLSL shader to SPIR-V:

```bash
$ glslangValidator -V shader.frag.glsl -o shader.frag.spv
$ glslangValidator -V shader.vert.glsl -o shader.vert.spv
```

Now you can convert the SPIR-V shader to other formats, for example to HLSL:

```bash
$ shadercross shader.vert.spv -o shader.frag.dxil
$ shadercross shader.frag.spv -o shader.frag.dxil
$ shadercross shader.vert.spv -o shader.frag.msl
$ shadercross shader.frag.spv -o shader.frag.msl
```

---

How to Setup RenderDoc
----------------------

**NOTE:** `renderdoc` doesn't support `Wayland` yet, so make sure you are running an X11 session (c.f https://wiki.libsdl.org/SDL3/FAQDevelopment#renderdoc).

To debug and inspect your shaders, you can use RenderDoc by Downloading it from (https://renderdoc.org/)[https://renderdoc.org/]:

```bash
$ cd linux_dxc_2025_07_14.x86_64 # The downloaded file
$ sudo cp -r bin/* /usr/local/bin/
$ sudo cp -r etc/* /usr/local/etc/
$ sudo cp -r include/* /usr/local/include/
$ sudo cp -r lib/* /usr/local/lib/
$ sudo cp -r lib/* /usr/local/share/
$ sudo ldconfig
```

Then you can run renderdoc:

```bash
$ qrenderdoc
```
