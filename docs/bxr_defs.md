<a name="line-1"></a>
File `bxr_defs.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Common definitions and macros

<a name="line-23"></a>
This is useful for providing default values for function parameters that are
passed as 0.

`val` is the value to check.

`def` is the default value to return if `val` is 0.

`return` `def` if `val` is 0, otherwise return `val`.

```cpp
#define BXR_DEFAULT(val, def) (((val) == 0) ? (def) : (val))
```

---

<a name="line-35"></a>
Get the offset of a struct member in bytes.

`TYPE` is the type of the struct.

`ELEMENT` is the name of the struct member.

`return` the offset of the struct member in bytes.

```cpp
#define BXR_OFFSET_OF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
```

---

<a name="line-46"></a>
Get the size of an array in elements.

`arr` is the array to get the size of.

`return` the size of the array in elements.

```cpp
#define BXR_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

---
