SDL3
----

SDL3 is a library that provides low-level access to audio, keyboard, mouse, joystick, and graphics hardware. It is widely used for game development and multimedia applications.

**Link:** [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL)
**Version:** 3.5.0

---

SDL3 Setup (Linux)
------------------

Building SDL3 from source ensures you have the latest version and can customize the build options as needed.

```bash
git clone https://github.com/libsdl-org/SDL.git
cd SDL

mkdir build
cd build

# Tweak those options as needed
cmake .. -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib64 -DCMAKE_BUILD_TYPE=Release
make

sudo make install
```
