/**
 * File `bxr_image_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Image (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_IMAGE_INTERNAL_H_
#define BXR_IMAGE_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_image.h"

bool bxr_image_setup(void);
void bxr_image_teardown(void);

#endif // BXR_IMAGE_INTERNAL_H_
