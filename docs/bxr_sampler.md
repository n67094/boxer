<a name="line-1"></a>
File `bxr_sampler.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Sampler Management for Texture Sampling in Shaders

<a name="line-30"></a>
## Internal API

<a name="line-34"></a>

```cpp
void bxr_sampler_setup(void)
```

Initializes the sampler system. Must be called before using any other sampler
functions.

---

<a name="line-40"></a>

```cpp
void bxr_sampler_shutdown(void)
```

Shuts down the sampler system.

---

<a name="line-45"></a>
## Public API

<a name="line-49"></a>

```cpp
bxr_sampler_t bxr_sampler_create(bxr_sampler_e sampler_type)
```

Create a sampler with the given sampler type.

`sampler_type` is the type of sampler to create.

`return` the created sampler, or an NULL sampler if the sampler could not
be created. Use `bxr_error_get` to get more information about the error.

---

<a name="line-59"></a>

```cpp
void bxr_sampler_destroy(bxr_sampler_t *sampler)
```

Destroy a sampler.

`sampler` is the sampler to destroy.

---
