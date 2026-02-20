Dear Bindings
-------------

Dear Bindings are C bindings for the popular Dear ImGui library (which is written in C++). They allow you to use ImGui in C projects, providing a way to create graphical user interfaces with ease.

**Link:** [https://github.com/dearimgui/dear_bindings](https://github.com/dearimgui/dear_bindings)

---

How to Update Sources
---------------------

The version of imgui is v1.92.2b from the docking branch.

Sources came from:

- dcimgui files where generate by [dcimgui](https://github.com/dearimgui/dear_bindings)
- imgui files where taken from [dear imgui](https://github.com/ocornut/imgui)

**NOTE:** "imgui_impl_sdlgpu3.h/cpp" files where generate using the command:

```
python dear_bindings.py --backend --include ../imgui-1.92/imgui.h -o dcimgui_impl_sdlgpu3 ../imgui/backends/imgui_impl_sdlgpu3.h 
```

