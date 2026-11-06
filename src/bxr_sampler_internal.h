/**
 * File `bxr_sampler_internal.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Sampler (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_SAMPLER_INTERNAL_H_
#define BXR_SAMPLER_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_sampler.h"

bool bxr_sampler_setup(void);
void bxr_sampler_teardown(void);

#endif // BXR_SAMPLER_INTERNAL_H_
