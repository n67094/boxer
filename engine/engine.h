/**
 * @file engine.h
 *
 * @brief The main header file for the Boxer Engine.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL3/SDL.h>

#include "engine_color.h"
#include "engine_config.h"
#include "engine_context.h"
#include "engine_defs.h"
#include "engine_error.h"
#include "engine_game.h"
#include "engine_gamepad.h"
#include "engine_image.h"
#include "engine_ini.h"
#include "engine_io.h"
#include "engine_json.h"
#include "engine_keyboard.h"
#include "engine_math.h"
#include "engine_mem.h"
#include "engine_mouse.h"
#include "engine_painter.h"
#include "engine_pipeline.h"
#include "engine_pool.h"
#include "engine_shader.h"
#include "engine_unit.h"

void engine_quit(void);

#endif // ENGINE_H_
