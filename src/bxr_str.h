/**
 * File `bxr_str.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # String Utilities
 */

#ifndef BXR_STR_H_
#define BXR_STR_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

// TODO proxy other SDL string functions if needed.

/**
 * Trim leading and trailing whitespace from the given string.
 *
 * `str` is the string to trim. The string is modified in-place.
 *
 * `return` the trimmed string, or NULL if the input string is NULL.
 */
BXR_INLINE char *
bxr_strtrim(char *str)
{
  if (!str)
    return str;

  // Trim leading whitespace
  while (*str && SDL_isblank(*str))
    str++;

  // Trim trailing whitespace
  size_t len = SDL_strlen(str);
  while (len > 0 && SDL_isblank(str[len - 1]))
    str[--len] = '\0'; // Null-terminate to remove trailing whitespace

  return str;
}

/**
 * Get the length of the initial segment of `str` which consists entirely of
 * characters not in `reject`.
 *
 * `str` is the string to search through.
 *
 * `reject` is the string containing the characters to reject.
 *
 * `return` the length of the initial segment of `str` which consists entirely
 * of characters not in `reject`.
 */
BXR_INLINE size_t
bxr_strcspn(const char *str, const char *reject)
{
  size_t len = 0;

  while (str[len] != '\0') {
    const char *r = reject;

    while (*r != '\0') {
      if (str[len] == *r) {
        return len;
      }
      r++;
    }
    len++;
  }
  return len; // End of str
}

#endif // BXR_STR_H_
