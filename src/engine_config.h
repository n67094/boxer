/**
 * @file engine_config.h
 *
 * @brief Engine configuration.
 *
 * @copyright Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef ENGINE_CONFIG_H_
#define ENGINE_CONFIG_H_

#include <SDL3/SDL.h>

// The directory where game assets are located (relative to the executable)
#define ENGINE_DATA_DIR "data"

// Max path length in characters
#define ENGINE_PATH_MAX 512

// Resources
// ---------------------------------------------------------------------------

// Max texture dimension (width or height) in pixels
#define ENGINE_RESOURCES_TEXTURE_DIMENSION_MAX 1024
// Max number of images that can be loaded at the same time
#define ENGINE_RESOURCES_IMAGE_MAX 64
// Max number of shaders that can be loaded at the same time
#define ENGINE_RESOURCES_SHADER_MAX 8
// Max number of pipelines that can be created at the same time
#define ENGINE_RESOURCES_PIPELINE_MAX 64
// Max number of sound that can be loaded at the same time
#define ENGINE_RESOURCES_SOUND_MAX 64

// Painter
// ---------------------------------------------------------------------------

// Max number of painters that can be created at the same time
#define ENGINE_PAINTER_MAX 16
// Max number of vertices that can be drawn in a single flush
#define ENGINE_PAINTER_VERTICES_MAX 65536
// Max number of draw commands that can be issued in a single flush
#define ENGINE_PAINTER_COMMANDS_MAX 16384
// Max number of transforms that can be pushed in the stack (per painter)
#define ENGINE_PAINTER_TRANSFORMS_MAX 256
// Max number of float uniforms that can be set in a single draw call
#define ENGINE_PAINTER_CONTENT_SLOTS_MAX 8
// TDDO not implemented yet
#define ENGINE_PAINTER_OPTIMIZE_MAX 0

#endif // ENGINE_CONFIG_H_
