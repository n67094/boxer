/**
 * File `bxr_config.h`.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 *
 * # Boxer Configuration
 */

#ifndef BXR_CONFIG_H_
#define BXR_CONFIG_H_

#include <SDL3/SDL.h>

/**
 * ## Global Configuration
 */

/**
 * Max path length in characters
 */
#define BXR_PATH_MAX_LENGTH 512

/**
 * ## Filesystem Configuration
 */

/**
 * The directory where game assets are located (relative to the executable)
 */
#define BXR_DATA_DIR "data"

/**
 * ## Painter Configuration
 */

/**
 * Max number of painters state that can be used simultaneously (per frame)
 */
#define BXR_PAINTER_MAX_STATE 16

/**
 * Max number of textures that can be bound at the same time (per draw call)
 */
#define BXR_PAINTER_MAX_TEXTURE_SLOTS 4

/**
 * Max number of transforms that can be pushed at the same time (per frame)
 */
#define BXR_PAINTER_MAX_TRANSFORMS 64

/**
 * Max number of float (4-bytes) uniforms that can be set in a shader
 */
#define BXR_PAINTER_MAX_UNIFORM_FLOATS 8

/**
 * Max number of commands that are looked back and batched together for
 * optimization
 */
#define BXR_PAINTER_OPTIMIZER_DEPTH 8

/**
 * ## Painter Resources Configuration
 */

/**
 * Max texture dimension in pixels
 */
#define BXR_PAINTER_MAX_TEXTURE_DIMENSION 4096

/**
 * Max number of images that can be loaded at the same time
 */
#define BXR_PAINTER_MAX_IMAGE 64

/**
 * Max number of shaders that can be loaded at the same time
 */
#define BXR_PAINTER_MAX_SHADER 8

/**
 * Max number of pipelines that can be created at the same time
 */
#define BXR_PAINTER_MAX_PIPELINE 64

/**
 * ## INI Reader/Writer Configuration
 */

/**
 * Max number of characters in a line of an INI file (including section headers,
 * key-value pairs, and comments).
 */
#define BXR_INI_MAX_ROW_SIZE 256

/**
 * Default initial capacity for INI data storage (in characters). The reader and
 * writer will automatically resize if the data exceeds this capacity.
 */
#define BXR_INI_DEFAULT_DATA_CAPACITY 512

/**
 * ## ECS Configuration
 */

/**
 * Default number of entities that can be created at the same time.
 */
#define BXR_ECS_DEFAULT_ENTITY 256

/**
 * max number of components that can be defined at the same time.
 */
#define BXR_ECS_MAX_COMPONENT 32

/**
 * max number of systems that can be defined at the same time.
 */
#define BXR_ECS_MAX_SYSTEM 16

/**
 * ## UI Configuration
 */

/* TODO re-add UI implementation

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
*/

#endif // BXR_CONFIG_H_
