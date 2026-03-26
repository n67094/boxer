/**
 * @file bxr_str.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_STR_H_
#define BXR_STR_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

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
