<a name="line-1"></a>
File `bxr_io.h`.

Copyright (c) 2026 nsix. All rights reserved.

# IO Utilities for Reading and Writing Files

File are `read` from `BXR_DATA_DIR` first then the system preferred path.

Depending on the operating system, the `system preferred path` is as follows:

- **Linux:** `~/.local/share/<config->name>/`
- **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
- **macOS:** `~/Library/Application Support/<config->name>/`

<a name="line-28"></a>
## 🔧 BXR_INLINE Uint8 *

```cpp
BXR_INLINE Uint8 *
```

Read a file from the given path and return its contents.

`path` is the path to the file to read, relative to the data directory.

`length` is an optional output parameter to store the length of the returned
data.

`return` the contents of the file or NULL if an error occurred. Use
`bxr_error_get` to get more information about the error.

The caller is responsible for freeing the returned data using `SDL_free` when
it is no longer needed.

<a name="line-94"></a>
## 🔧 BXR_INLINE bool

```cpp
BXR_INLINE bool
```

Write data to a file at the given path.

`path` is the path to the file to write, relative to the data directory.

`data` is a pointer to the data to write.

`length` is the length of the data in bytes.

`append` is a boolean flag indicating whether to append to the file (if true)
or overwrite it (if false).

`return` true if the write operation was successful, false otherwise. Use
`bxr_error_get` to get more information about the error if the write
