<a name="line-1"></a>
File `bxr_str.h`.

Copyright (c) 2026 nsix. All rights reserved.

# String Utilities

<a name="line-90"></a>
## 🔧 BXR_INLINE char *

```cpp
BXR_INLINE char *
```

Trim leading and trailing whitespace from the given string.

`str` is the string to trim. The string is modified in-place.

`return` the trimmed string, or NULL if the input string is NULL.

<a name="line-115"></a>
## 🔧 BXR_INLINE size_t

```cpp
BXR_INLINE size_t
```

Get the length of the initial segment of `str` which consists entirely of
characters not in `reject`.

`str` is the string to search through.

`reject` is the string containing the characters to reject.

`return` the length of the initial segment of `str` which consists entirely
of characters not in `reject`.
