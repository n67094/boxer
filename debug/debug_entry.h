/**
 * @file debug_entry.c
 *
 * @brief Debug interface entry point.
 *
 * @details This module provides lifecycle functions for setting up,
 * rendering, and shutting down the debug interface.
 *
 * It's here that all ImGui windows and widgets are managed.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef DEBUG_ENTRY_H_
#define DEBUG_ENTRY_H_

#include "engine.h"

void debug_entry_setup(void);

void debug_entry_frame(Uint64 ups, Uint64 fps);

void debug_entry_shutdown(void);

#endif // DEBUG_ENTRY_H_
