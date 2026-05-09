<a name="line-1"></a>
File `bxr_ini.h`.

Copyright (c) 2026 nsix. All rights reserved.

# INI File Reader and Writer

Support strings and numbers. Booleans are not supported, but you can use `0`
and `1` to represent false and true respectively.

File extension doesn't matter, but recommended to use `.ini`.

Example INI file:

```
[Window] ; comment
title="Test"
width=800
height=600

; comment

[Player]
name="nsix\ttest" ; Please note that the name contains a tab character
test= ; this will be ignored
```

<a name="line-36"></a>
## Reader API

<a name="line-40"></a>
---

```cpp
bxr_ini_reader_t *bxr_ini_create_reader(const char *path)
```

Load an INI file from the given path and return a reader for it.

`path` is the path to the INI file to load. (c.f `bxr_io_read` for path
resolution).

`return` a reader for the loaded INI file, or NULL if an error occurred. Use
`bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned reader using
`bxr_ini_destroy_reader` when it is no longer needed.

<a name="line-54"></a>
---

```cpp
void bxr_ini_destroy_reader(bxr_ini_reader_t *ini)
```

Destroy an INI reader and free its resources.

`ini` is the INI reader to destroy.

<a name="line-61"></a>
---

```cpp
const char *
```

Get the string value associated with the given section and key.

`ini` is the INI reader to query.

`section` is the section name to look for.

`key` is the key name to look for within the section.

`return` the string value associated with the given section and key, or NULL
if the section or key was not found.

<a name="line-76"></a>
---

```cpp
float bxr_ini_read_number(bxr_ini_reader_t *ini,
```

Get the number value associated with the given section and key.

`ini` is the INI reader to query.

`section` is the section name to look for.

`key` is the key name to look for within the section.

`return` the number value associated with the given section and key, or 0.0
if the section or key was not found.

<a name="line-92"></a>
---

```cpp
const char *bxr_ini_read_str_or_else(bxr_ini_reader_t *ini,
```

Get the string value associated with the given section and key, or return a
default value if the section or key was not found.

`ini` is the INI reader to query.

`section` is the section name to look for.

`key` is the key name to look for within the section.

`default_value` is the default value to return if the section or key was not
found.

`return` the string value associated with the given section and key, or
`default_value` if the section or key was not found.

<a name="line-113"></a>
---

```cpp
float bxr_ini_read_number_or_else(bxr_ini_reader_t *ini,
```

Get the number value associated with the given section and key, or return a
default value if the section or key was not found.

`ini` is the INI reader to query.

`section` is the section name to look for.

`key` is the key name to look for within the section.

`default_value` is the default value to return if the section or key was not
found.

`return` the number value associated with the given section and key, or
`default_value` if the section or key was not found.

<a name="line-134"></a>
## Writer API

<a name="line-140"></a>
---

```cpp
bxr_ini_writer_t *bxr_ini_create_writer(void)
```

Create a new INI writer.

`return` a new INI writer, or NULL an error occurred. Use `bxr_error_get` to
get more information about the error.

The caller is responsible for destroying the returned writer using
`bxr_ini_destroy_writer` when it is no longer needed.

<a name="line-151"></a>
---

```cpp
void bxr_ini_destroy_writer(bxr_ini_writer_t *ini)
```

Destroy an INI writer and free its resources.

`ini` is the INI writer to destroy.

<a name="line-158"></a>
---

```cpp
bool bxr_ini_writer_section_begin(bxr_ini_writer_t *ini, const char *section)
```

Begin a new section in the INI writer.

`ini` is the INI writer to use.

`section` is the name of the section to begin.

`return` true if the section was successfully begun, or false if an error
occurred. Use `bxr_error_get` to get more information about the error.

<a name="line-170"></a>
---

```cpp
bool bxr_ini_writer_section_end(bxr_ini_writer_t *ini)
```

End the current section in the INI writer.

`ini` is the INI writer to use.

`return` true if the section was successfully ended, or false if an error
occurred. Use `bxr_error_get` to get more information about the error.

<a name="line-180"></a>
---

```cpp
bool
```

Add a key-value pair with a string value to the current section in the INI
writer.

`ini` is the INI writer to use.

`key` is the name of the key to add.

`value` is the string value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred. Use `bxr_error_get` to get more information about the error.

<a name="line-196"></a>
---

```cpp
bool bxr_ini_write_number(bxr_ini_writer_t *ini, const char *key, float number)
```

Add a key-value pair with a number value to the current section in the INI
writer.

`ini` is the INI writer to use.

`key` is the name of the key to add.

`number` is the number value to associate with the key.

`return` true if the key-value pair was successfully added, or false if an
error occurred. Use `bxr_error_get` to get more information about the error.

<a name="line-211"></a>
---

```cpp
bool bxr_ini_writer_save(bxr_ini_writer_t *ini, const char *path)
```

Save the INI data to a file at the given path.

`ini` is the INI writer containing the data to save.

`path` is the path to the file to save. (c.f `bxr_io_write` for path
resolution).

`return` true if the INI data was successfully saved, or false if an error
occurred. Use `bxr_error_get` to get more information about the error.
