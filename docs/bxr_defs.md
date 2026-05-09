<a name="line-1"></a>
File `bxr_defs.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Common definitions and macros

<a name="line-23"></a>

```cpp
#define BXR_DEFAULT(val, def) (((val) == 0) ? (def) : (val))
```

This is useful for providing default values for function parameters that are
passed as 0.

`val` is the value to check.

`def` is the default value to return if `val` is 0.

`return` `def` if `val` is 0, otherwise return `val`.
---

<a name="line-35"></a>

```cpp
#define BXR_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
```

Get the offset of a struct member in bytes.

`TYPE` is the type of the struct.

`ELEMENT` is the name of the struct member.

`return` the offset of the struct member in bytes.
---

<a name="line-46"></a>

```cpp
#define BXR_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

Get the size of an array in elements.

`arr` is the array to get the size of.

`return` the size of the array in elements.
---
