<a name="line-1"></a>
// TODO refactor documentation, add bxr_text.h function here
@file bxr_font.h

@brief Font utilities.

@details ASCII characters are supported.

Font are defined by an array of rectangles that specify the position and
size of each chars or icons within an image. Icons and chars are defined by
a range of indices. For example, if you have 4 icons and 96 chars, you can
set the icon range to (0, 3) and the char range to (32, 126).

(You can use rxi atlas generator to generate the atlas and the corresponding
glyphs array: https://github.com/rxi/atlas)

@copyright Copyright (c) 2026 nsix. All rights reserved.
