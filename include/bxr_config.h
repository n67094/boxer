/**
 * File `bxr_config.h`.
 *
 * # Configuration.
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
 */
#define BXR_PATH_MAX_LENGTH 512

/**
 * Max length of an error message in bytes, including the NUL terminator.
 * Longer messages are truncated.
 */
#define BXR_ERROR_MAX_LENGTH 1024

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
 * Max number of draw call that are looked back and batched together.
 */
#define BXR_PAINTER_MAX_OPTIMIZER_DEPTH 8

/**
 * ## Painter Resources Configuration
 */

/**
 * Max texture width or height in pixels.
 */
#define BXR_PAINTER_MAX_TEXTURE_SIZE (16 * 1024 * 1024) // 16 mb

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
 * ## Audio Configuration
 */

/**
 * Max number of audio that can be loaded at the same time.
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
 * Size of the audio chunk fed to a playing voice's stream.
 *
 * 2048 frames * 2 channels * 4 bytes = 16 KB per chunk.
 */
#define BXR_AUDIO_LOAD_FRAMES 2048

/**
 * ## INI Reader/Writer Configuration
 */

/**
 * Initial capacity in bytes of the INI writer buffer. The buffer doubles in
 * size when it is full.
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

#endif // BXR_CONFIG_H_
