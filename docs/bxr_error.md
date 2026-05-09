<a name="line-1"></a>
File `bxr_error.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Error handling system

<a name="line-36"></a>
## Internal API

<a name="line-40"></a>
Set the last error code.

`error` is the error code to set.

```cpp
void bxr_error_set(bxr_erro_code_e error)
```

---

<a name="line-47"></a>
## Public API

<a name="line-51"></a>
Get the last error code.

`return` the last error code.

```cpp
bxr_erro_code_e bxr_error_get(void)
```

---

<a name="line-58"></a>
Get the error message corresponding to the given error code.

`error` is the error code to get the message for.

`return` the error message corresponding to the given error code.

```cpp
const char *bxr_error_get_msg(bxr_erro_code_e error)
```

---
