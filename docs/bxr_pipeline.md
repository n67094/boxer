<a name="line-1"></a>
File `bxr_pipeline.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Graphics Pipeline Management

<a name="line-40"></a>
## Public API

<a name="line-44"></a>

```cpp
bxr_pipeline_t bxr_pipeline_create(bxr_shader_t shader,
```

Create a graphics pipeline with the given shader, primitive type, and blend
mode.

`shader` is the shader to use for the pipeline.

`primitive_type` is the type of primitives to render with the pipeline.

`blend_mode` is the blend mode to use for rendering with the pipeline.

`return` the created pipeline, or an invalid pipeline if the pipeline could
not be created.

The caller is responsible for destroying the returned pipeline using
`bxr_pipeline_destroy` when it is no longer needed.

---

<a name="line-64"></a>

```cpp
void bxr_pipeline_destroy(bxr_pipeline_t pipeline)
```

Destroy a graphics pipeline.

`pipeline` is the pipeline to destroy.

---
