/**
 * File `bxr.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Boxer Main Header File
 */

#ifndef BXR_H_
#define BXR_H_

#include <SDL3/SDL.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_ecs.h"
#include "bxr_game.h"
#include "bxr_gamepad.h"
#include "bxr_image.h"
#include "bxr_ini.h"
#include "bxr_json.h"
#include "bxr_keyboard.h"
#include "bxr_log.h"
#include "bxr_math.h"
#include "bxr_mem.h"
#include "bxr_mouse.h"
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_sampler.h"
#include "bxr_shader.h"
#include "bxr_sparse_set.h"
#include "bxr_str.h"
#include "bxr_time.h"
#include "bxr_unit.h"

/**
 * Quit the game loop and exit the application.
 */
void bxr_quit(void);

#endif // BXR_H_
