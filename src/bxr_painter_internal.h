/**
 * File `bxr_painter_internal.h`.
 *
 * Copyright (c) 2025 - 2026 nsix. All rights reserved.
 *
 * # Painter (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_PAINTER_INTERNAL_H_
#define BXR_PAINTER_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_painter.h"

typedef struct bxr_painter_desc_t
{
  Uint32 max_vertices;
  Uint32 max_commands;
  SDL_Window *window;
  SDL_GPUDevice *gpu_device;
} bxr_painter_desc_t;

bool bxr_painter_setup(const bxr_painter_desc_t *desc);
void bxr_painter_teardown(void);

#endif // BXR_PAINTER_INTERNAL_H_
