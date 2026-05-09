<a name="line-1"></a>
File `bxr_shader.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Shader Management System.

Shader bytecode are loaded from `BXR_DATA_DIR`.

Shader should be name like this `<name>.<stage>.<format>`, where:

- `<name>` is the name of the shader (e.g. "basic", "sprite", etc.).
- `<stage>` is the shader stage (e.g. "vert" for vertex shader, "frag" for
fragment shader).
- `<format>` is the shader bytecode format (e.g. "spv" for SPIR-V, "msl" for
Metal Shading Language, etc.).

<a name="line-49"></a>
## Internal API

<a name="line-53"></a>
Initializes the shader system. Must be called before using any other shader
functions.

`context` is the bxr context to use for shader operations.

```cpp
void bxr_shader_setup(bxr_context_t *context)
```

---

<a name="line-61"></a>
Shuts down the shader system.

```cpp
void bxr_shader_shutdown(void)
```

---

<a name="line-66"></a>
## Public API

<a name="line-70"></a>
Creates a shader from the given description.

`desc` is the description of the shader to create.

`return` the created shader, or an invalid shader if the shader could not
be created. Use `bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned shader using
`bxr_shader_destroy` when it is no longer needed.

```cpp
bxr_shader_t bxr_shader_create(bxr_shader_desc_t *desc)
```

---

<a name="line-83"></a>
Destroys a shader, freeing any resources associated with it.

`shader` is the shader to destroy.

```cpp
void bxr_shader_destroy(bxr_shader_t shader)
```

---
