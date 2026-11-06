/**
 * File `bxr_audio_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Audio (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_AUDIO_INTERNAL_H_
#define BXR_AUDIO_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_audio.h"

bool bxr_audio_setup(void);
void bxr_audio_begin_frame(void);
void bxr_audio_teardown(void);

#endif // BXR_AUDIO_INTERNAL_H_
