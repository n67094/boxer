/**
 * File `bxr_config.h`.
 *
 * Compile-time limits and default sizes used by boxer modules.
 *
 * Values are fixed at build time; change them here and rebuild boxer.
 *
 * Copyright (c) 2025 nsix. All rights reserved.
 */

#ifndef BXR_CONFIG_H_
#define BXR_CONFIG_H_

#include <SDL3/SDL.h>

/**
 * ## Global Configuration
 */

/**
 * Max length of a file path in bytes, including the NUL terminator.
 *
 * Longer paths are truncated.
 */
#define BXR_PATH_MAX_LENGTH 512

/**
 * ## Painter Configuration
 */

/**
 * Max number of painter states that can be used at the same time (per frame).
 */
#define BXR_PAINTER_MAX_STATE 16

/**
 * Max number of textures that can be bound at the same time (per draw call).
 */
#define BXR_PAINTER_MAX_TEXTURE_SLOTS 4

/**
 * Max number of transforms that can be pushed at the same time (per frame).
 */
#define BXR_PAINTER_MAX_TRANSFORMS 64

/**
 * Max number of float (4 bytes) uniforms that can be set in a shader.
 */
#define BXR_PAINTER_MAX_UNIFORM_FLOATS 8

/**
 * Max number of commands that are looked back and batched together for
 * optimization.
 *
 * FIXME: unused and duplicates `BXR_PAINTER_MAX_OPTIMIZER_DEPTH`, which is the
 * value actually forwarded to SDL_gp. Remove one of the two.
 */
#define BXR_PAINTER_OPTIMIZER_DEPTH 8

/**
 * ## Painter Resources Configuration
 */

/**
 * Max texture width or height in pixels.
 *
 * FIXME: forwarded as `SDL_GP_TEXTURE_DIMENSION_MAX`, which SDL_gp never reads
 * (it uses `SDL_GP_TEXTURE_SIZE_MAX`, a transfer buffer size in bytes), so
 * this value has no effect. Map it to the right SDL_gp option or remove it.
 */
#define BXR_PAINTER_MAX_TEXTURE_DIMENSION 4096

/**
 * Max number of images that can be loaded at the same time.
 */
#define BXR_PAINTER_MAX_IMAGE 64

/**
 * Max number of shaders that can be loaded at the same time.
 */
#define BXR_PAINTER_MAX_SHADER 8

/**
 * Max number of pipelines that can be created at the same time.
 */
#define BXR_PAINTER_MAX_PIPELINE 64

/**
 * Max number of commands that are looked back and batched together for
 * optimization.
 */
#define BXR_PAINTER_MAX_OPTIMIZER_DEPTH 8

/**
 * ## Audio Configuration
 */

/**
 * Max number of audio tracks that can be loaded at the same time.
 */
#define BXR_AUDIO_MAX 32

/**
 * Max number of voices that can be played at the same time.
 */
#define BXR_AUDIO_MAX_VOICES 32

/**
 * Max number of tags that can be associated with a voice.
 */
#define BXR_AUDIO_MAX_TAGS 8

/**
 * Target audio latency in milliseconds.
 *
 * Each voice keeps about this much audio queued in its stream.
 */
// FIXME convert this to bytes
#define BXR_AUDIO_TARGET_LATENCY_MS 50

/**
 * ## INI Reader/Writer Configuration
 */

/**
 * Max number of characters in a line of an INI file (including section headers,
 * key-value pairs, and comments).
 *
 * FIXME: unused, the INI reader does not enforce any line length. Enforce it
 * or remove it.
 */
#define BXR_INI_MAX_ROW_SIZE 256

/**
 * Initial capacity in bytes of the INI writer buffer.
 *
 * The buffer doubles in size when it is full.
 */
#define BXR_INI_DEFAULT_DATA_CAPACITY 512

/**
 * ## ECS Configuration
 */

/**
 * Initial entity capacity used by `bxr_ecs_create()` when `entity_count` is 0.
 */
#define BXR_ECS_DEFAULT_ENTITY 256

/**
 * Max number of components that can be registered in an ECS instance.
 */
#define BXR_ECS_MAX_COMPONENT 32

/**
 * Max number of systems that can be registered in an ECS instance.
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
