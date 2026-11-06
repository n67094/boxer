/**
 * File `bxr_log_internal.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Log (internal)
 *
 * Engine-only declarations, not installed with the public headers.
 */

#ifndef BXR_LOG_INTERNAL_H_
#define BXR_LOG_INTERNAL_H_

#include <SDL3/SDL.h>

#include "bxr_log.h"

void bxr_log_setup(void);
void bxr_log_teardown(void);

#endif // BXR_LOG_INTERNAL_H_
