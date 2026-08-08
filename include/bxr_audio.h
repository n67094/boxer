#ifndef BXR_AUDIO_H_
#define BXR_AUDIO_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

typedef struct bxr_audio_s
{
  Uint32 id;
} bxr_audio_t;

BXR_API bxr_audio_t bxr_audio_create(const char *path);

#endif // BXR_AUDIO_H_
