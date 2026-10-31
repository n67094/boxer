/**
 * File `bxr.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Main Header
 *
 * Boxer owns the program entry point through SDL main callbacks: do
 * not write a `main()`, implement the callbacks of `bxr_game.h` instead.
 */

#ifndef BXR_H_
#define BXR_H_

#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_audio.h"
#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_error.h"
#include "bxr_game.h"
#include "bxr_gamepad.h"
#include "bxr_image.h"
#include "bxr_ini.h"
#include "bxr_keyboard.h"
#include "bxr_log.h"
#include "bxr_math.h"
#include "bxr_mem.h"
#include "bxr_mouse.h"
#include "bxr_multimap.h"
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_pixel.h"
#include "bxr_sampler.h"
#include "bxr_shader.h"
#include "bxr_sparse_set.h"
#include "bxr_string.h"
#include "bxr_time.h"
#include "bxr_unit.h"

/**
 * ## API
 */

/**
 * Request the application to exit.
 *
 * Asynchronous: pushes an `SDL_EVENT_QUIT` and returns immediately. The
 * current frame finishes, then `bxr_game_teardown()` and the subsystem
 * teardowns run before the process exits. Safe to call from any thread.
 */
void bxr_quit(void);

#endif // BXR_H_
