/**
 * File `bxr_shader_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Shader (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_SHADER_INTERNAL_H_
#define BXR_SHADER_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_shader.h"
#include "bxr_context_internal.h"

bool bxr_shader_setup(bxr_context_t *context);
void bxr_shader_teardown(void);

#endif // BXR_SHADER_INTERNAL_H_
