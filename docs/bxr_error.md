<a name="line-1"></a>
File `bxr_error.h`.

Copyright (c) 2025 nsix. All rights reserved.

# Error handling system

<a name="line-36"></a>
## Internal API

<a name="line-40"></a>
### 🔹 void bxr_error_set(bxr_erro_code_e error)

```cpp
void bxr_error_set(bxr_erro_code_e error)
```

Set the last error code.

`error` is the error code to set.

<a name="line-47"></a>
## Public API

<a name="line-51"></a>
### 🔹 bxr_erro_code_e bxr_error_get(void)

```cpp
bxr_erro_code_e bxr_error_get(void)
```

Get the last error code.

`return` the last error code.

<a name="line-58"></a>
### 🔹 const char *bxr_error_get_msg(bxr_erro_code_e error)

```cpp
const char *bxr_error_get_msg(bxr_erro_code_e error)
```

Get the error message corresponding to the given error code.

`error` is the error code to get the message for.

`return` the error message corresponding to the given error code.
