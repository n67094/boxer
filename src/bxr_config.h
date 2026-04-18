/**
 * file `bxr_config.h`
 *
 * copyright Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Boxer configuration.
 */

// TODO SDL_gp config override ?

#ifndef BXR_CONFIG_H_
#define BXR_CONFIG_H_

#include <SDL3/SDL.h>

// The directory where game assets are located (relative to the executable)
#define BXR_DATA_DIR "data"

// Max path length in characters
#define BXR_PATH_MAX 512

// Resources
// ---------------------------------------------------------------------------

// Max texture dimension (width or height) in pixels
#define BXR_RESOURCES_TEXTURE_DIMENSION_MAX 1024
// Max number of images that can be loaded at the same time
#define BXR_RESOURCES_IMAGE_MAX 64
// Max number of shaders that can be loaded at the same time
#define BXR_RESOURCES_SHADER_MAX 8
// Max number of pipelines that can be created at the same time
#define BXR_RESOURCES_PIPELINE_MAX 64
// Max number of sound that can be loaded at the same time
#define BXR_RESOURCES_SOUND_MAX 64

// Painter
// ---------------------------------------------------------------------------

// Max number of painters state that can be used simultaneously (per frame)
#define BXR_PAINTER_STATE_MAX 16
// Max number of textures that can be bound at the same time (per draw call)
#define BXR_PAINTER_TEXTURE_SLOTS_MAX 4
// Max number of vertices that can be drawn in a single flush
#define BXR_PAINTER_VERTICES_MAX 65536
// Max number of draw commands that can be issued in a single flush
#define BXR_PAINTER_COMMANDS_MAX 16384
// Max number of transforms that can be pushed at the same time (per frame)
#define BXR_PAINTER_TRANSFORMS_MAX 256
// Max number of float uniforms that can be set in a single draw call
#define BXR_PAINTER_CONTENT_SLOTS_MAX 8
// TDDO not implemented yet
#define BXR_PAINTER_OPTIMIZE_MAX 0

// UI
// ---------------------------------------------------------------------------

// Max number of painters state that can be used simultaneously (per frame)
#define BXR_UI_STATE_MAX 16
// Max number of commands that can be issued in a single flush (per ui state)
#define BXR_UI_COMMAND_MAX 256
// Max number of root containers (e.g window without parent) that can use in the
// same time (per ui state)
#define BXR_UI_ROOT_MAX 32
// Max number of clip that can be used at the same time (per ui state)
#define BXR_UI_CLIP_MAX 32
// Max number of id that can be used at the same time (per ui state)
#define BXR_UI_ID_MAX 32
// Max number of containers (root and non-root) that can be created at the same
// time (per ui state)
#define BXR_UI_CONTAINER_SLOTS_MAX 32
// Max number of items in a layout row
#define BXR_UI_LAYOUT_SLOTS_MAX 32

// Atom
// ---------------------------------------------------------------------------

// Max number of unique strings
#define BXR_ATOM_MAX 2048

#endif // BXR_CONFIG_H_
