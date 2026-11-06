/**
 * File `bxr.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Main Header
 *
 * Boxer is a lightweight 2D game framework built for rapid development of
 * simple games. It offers a straightforward API for graphics, input, and core
 * game mechanics, which makes it a great fit for beginners and hobbyists.
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
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_pixel.h"
#include "bxr_sampler.h"
#include "bxr_shader.h"
#include "bxr_string.h"
#include "bxr_time.h"
#include "bxr_unit.h"

/**
 * ## API
 */

/**
 * Request the application to exit.
 */
void bxr_quit(void);

#endif // BXR_H_
