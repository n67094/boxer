/**
 * File `bxr_sdl_gp.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # SDL_gp Configuration
 *
 * Includes SDL_gp configured with boxer limits from `bxr_config.h`.
 *
 * Always include this header instead of `<SDL_gp.h>`: the `SDL_GP_*` limits
 * must be defined before SDL_gp is included, otherwise its defaults are used.
 */

#ifndef BXR_SDL_GP_H_
#define BXR_SDL_GP_H_

#include "bxr_config.h"

// Override SDL_gp configuration.

#define SDL_GP_PATH_MAX BXR_PATH_MAX_LENGTH

#define SDL_GP_IMAGE_MAX BXR_PAINTER_MAX_IMAGE

#define SDL_GP_SHADER_MAX BXR_PAINTER_MAX_SHADER

#define SDL_GP_PIPELINE_MAX BXR_PAINTER_MAX_PIPELINE

#define SDL_GP_STATE_MAX BXR_PAINTER_MAX_STATE

#define SDL_GP_OPTIMIZER_DEPTH BXR_PAINTER_MAX_OPTIMIZER_DEPTH

#define SDL_GP_TEXTURE_SIZE_MAX BXR_PAINTER_MAX_TEXTURE_SIZE

#define SDL_GP_TEXTURE_SLOTS_MAX BXR_PAINTER_MAX_TEXTURE_SLOTS

#define SDL_GP_TRANSFORMS_MAX BXR_PAINTER_MAX_TRANSFORMS

#define SDL_GP_UNIFORM_FLOATS_MAX BXR_PAINTER_MAX_UNIFORM_FLOATS

#include <SDL_gp.h>

#endif // BXR_SDL_GP_H_
