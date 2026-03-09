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

To install the `DirectXShaderCompile` dependencies [https://github.com/microsoft/DirectXShaderCompiler/releases](https://github.com/microsoft/DirectXShaderCompiler/releases), download the latest linux version extract and run the following commands:

```bash
cp -r bin/* ~/.local/bin/
cp -r lib/* ~/.local/lib/
cp -r include/* ~/.local/include/

sudo ldconfig ~/.local/lib

echo 'export PATH="$PATH:~/.local/bin"' >> ~/.bashrc
echo 'export CMAKE_PREFIX_PATH="~/.local:$CMAKE_PREFIX_PATH"' >> ~/.bashrc
echo 'export PKG_CONFIG_PATH="~/.local/share/pkgconfig:$PKG_CONFIG_PATH"' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH="~/.local/lib:$LD_LIBRARY_PATH"' >> ~/.bashrc

source ~/.bashrc
 ```

 Test with: 

```bash
dxc --version
```

Finally to install `SDL_shadercross` run those commands:

```bash
git clone https://github.com/libsdl-org/SDL_shadercross
cd SDL_shadercross
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cp -r shadercross ~/.local/bin/
```

Test with:

```bash
shadercross -h
```

How to Compile shaders
----------------------

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
