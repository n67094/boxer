# Docgen

Generate documentation to markdown from C header files.

## Usage

```bash
node docgen -o output.md header.h
```

## Tags

`@self`: the declaration is self-explanatory, it is documented without a
description. Put it in a one-line comment, or on its own line in a comment
block. Consecutive `#define` lines after it are documented together.

```c
/** @self */
#define BXR_COLOR_WHITE (bxr_color_t){ .r = 255, .g = 255, .b = 255, .a = 255 }
#define BXR_COLOR_BLACK (bxr_color_t){ .r = 0, .g = 0, .b = 0, .a = 255 }
```
