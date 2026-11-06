/**
 * File `bxr_asset_internal.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Asset (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_ASSET_INTERNAL_H_
#define BXR_ASSET_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_asset.h"
#include "bxr_context_internal.h"

bool bxr_asset_setup(bxr_context_t *context);
void bxr_asset_teardown(void);

#endif // BXR_ASSET_INTERNAL_H_
