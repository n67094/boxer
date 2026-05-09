<a name="line-1"></a>
File `bxr_config.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Boxer Configuration

<a name="line-14"></a>
## Global Configuration

<a name="line-18"></a>

```cpp
#define BXR_PATH_MAX_LENGTH 512
```

Max path length in characters

---

<a name="line-23"></a>
## Filesystem Configuration

<a name="line-27"></a>

```cpp
#define BXR_DATA_DIR "data"
```

The directory where game assets are located (relative to the executable)

---

<a name="line-32"></a>
## Painter Configuration

<a name="line-36"></a>

```cpp
#define BXR_PAINTER_MAX_STATE 16
```

Max number of painters state that can be used simultaneously (per frame)

---

<a name="line-41"></a>

```cpp
#define BXR_PAINTER_MAX_TEXTURE_SLOTS 4
```

Max number of textures that can be bound at the same time (per draw call)

---

<a name="line-46"></a>

```cpp
#define BXR_PAINTER_MAX_TRANSFORMS 64
```

Max number of transforms that can be pushed at the same time (per frame)

---

<a name="line-51"></a>

```cpp
#define BXR_PAINTER_MAX_UNIFORM_FLOATS 8
```

Max number of float (4-bytes) uniforms that can be set in a shader

---

<a name="line-56"></a>

```cpp
#define BXR_PAINTER_OPTIMIZER_DEPTH 8
```

Max number of commands that are looked back and batched together for
optimization

---

<a name="line-62"></a>
## Painter Resources Configuration

<a name="line-66"></a>

```cpp
#define BXR_PAINTER_MAX_TEXTURE_DIMENSION 4096
```

Max texture dimension in pixels

---

<a name="line-71"></a>

```cpp
#define BXR_PAINTER_MAX_IMAGE 64
```

Max number of images that can be loaded at the same time

---

<a name="line-76"></a>

```cpp
#define BXR_PAINTER_MAX_SHADER 8
```

Max number of shaders that can be loaded at the same time

---

<a name="line-81"></a>

```cpp
#define BXR_PAINTER_MAX_PIPELINE 64
```

Max number of pipelines that can be created at the same time

---

<a name="line-86"></a>
## INI Reader/Writer Configuration

<a name="line-90"></a>

```cpp
#define BXR_INI_MAX_ROW_SIZE 256
```

Max number of characters in a line of an INI file (including section headers,
key-value pairs, and comments).

---

<a name="line-96"></a>

```cpp
#define BXR_INI_DEFAULT_DATA_CAPACITY 512
```

Default initial capacity for INI data storage (in characters). The reader and
writer will automatically resize if the data exceeds this capacity.

---

<a name="line-102"></a>
## ECS Configuration

<a name="line-106"></a>

```cpp
#define BXR_ECS_DEFAULT_ENTITY 256
```

Default number of entities that can be created at the same time.

---

<a name="line-111"></a>

```cpp
#define BXR_ECS_MAX_COMPONENT 32
```

max number of components that can be defined at the same time.

---

<a name="line-116"></a>

```cpp
#define BXR_ECS_MAX_SYSTEM 16
```

max number of systems that can be defined at the same time.

---

<a name="line-121"></a>
## UI Configuration
