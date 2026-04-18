/**
 * file bxr.h
 *
 * The main header file for the Boxer Engine.
 *
 * copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_H_
#define BXR_H_

#include <SDL3/SDL.h>

#include "bxr_color.h"
#include "bxr_config.h"
#include "bxr_context.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_font.h"
#include "bxr_game.h"
#include "bxr_gamepad.h"
#include "bxr_image.h"
#include "bxr_ini.h"
#include "bxr_io.h"
#include "bxr_json.h"
#include "bxr_keyboard.h"
#include "bxr_math.h"
#include "bxr_mem.h"
#include "bxr_mouse.h"
#include "bxr_painter.h"
#include "bxr_pipeline.h"
#include "bxr_pool.h"
#include "bxr_shader.h"
#include "bxr_text.h"
#include "bxr_unit.h"

void bxr_quit(void);

#endif // BXR_H_
