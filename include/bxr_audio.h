/**
 * File `bxr_audio.h`.
 *
 * Audio loading and playback module for the Boxer engine.
 *
 * Typical usage: `bxr_audio_create()`, then `bxr_voice_create()` for as many
 * voices as you need, then `bxr_voice_play()`, `bxr_voice_pause()`, etc.
 * Once done, call `bxr_voice_destroy()` and `bxr_audio_destroy()` to free the
 * resources.
 *
 * When the number of voices exceeds `BXR_AUDIO_MAX_VOICES`, the oldest voice is
 * stopped and destroyed to make room for the new one. This is a simple voice
 * management strategy. So any handle to a voice may become invalid, operations
 * like `bxr_voice_set_volume()` will then do nothing. `bxr_voice_validity()`
 * can be used to check if a voice is still valid.
 *
 * //TODO
 *
 * 1. when adding a voice save the frame number.
 * 2. line 1011: if the multimap is full, remove the oldest voice from
 * voice_pool_ and destroy it (this should remove it from the multimap too).
 * 3. Then add the new voice to the multimap.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_AUDIO_H_
#define BXR_AUDIO_H_

#include <SDL3/SDL.h>

#include "bxr_defs.h"

#define BXR_AUDIO_INVALID_ID 0
#define BXR_VOICE_INVALID_ID 0

/**
 * Handle to loaded audio data.
 */
typedef struct bxr_audio_s
{
  Uint32 id;
} bxr_audio_t;

/**
 * Handle to a playback voice.
 */
typedef struct bxr_voice_s
{
  Uint32 id;
} bxr_voice_t;

/**
 * Parameters for `bxr_voice_create()`.
 *
 * `audio` the audio to play; must be valid.
 * `volume` initial volume, clamped to [0.0, 1.0].
 * `auto_play` start playing immediately when true.
 * `play_once` play a single time and then destroy the voice automatically;
 * when false the voice loops.
 */
typedef struct bxr_voice_desc_s
{
  bxr_audio_t audio;
  float volume;
  bool auto_play;
  bool play_once;
} bxr_voice_desc_t;

/**
 * ## Internal API
 */

void bxr_audio_setup(void);
void bxr_audio_begin_frame(void);
void bxr_audio_teardown(void);

/**
 * ## Public API
 */

/**
 * Load a WAV or OGG file into memory.
 *
 * The format is chosen from the file extension (`.wav` or `.ogg`).
 *
 * `path` file path relative to the mounted directories; must not be NULL.
 *
 * `return` the audio handle, or an id of `BXR_AUDIO_INVALID_ID` on failure.
 *
 * **Note**: the caller must free the audio with `bxr_audio_destroy()`.
 */
BXR_API bxr_audio_t bxr_audio_create(const char *path);

/**
 * Get the duration of an audio.
 *
 * `audio` the audio to query.
 *
 * `return` the duration in milliseconds, or 0 if the audio is invalid.
 */
BXR_API Uint64 bxr_audio_get_length(bxr_audio_t audio);

/**
 * Free an audio and its sample data.
 *
 * `audio` the audio to destroy; invalid handles are logged and ignored.
 *
 * **NOTE**: Every voice playing this audio is destroyed too and their handles
 * become invalid.
 */
BXR_API void bxr_audio_destroy(bxr_audio_t audio);

/**
 * ## Global controls
 */

/**
 * Set the master volume of the playback device.
 *
 * `volume` gain clamped to [0.0, 1.0].
 */
BXR_API void bxr_audio_set_global_volume(float volume);

/**
 * Get the master volume of the playback device.
 *
 * `return` the current gain.
 */
BXR_API float bxr_audio_get_global_volume(void);

/**
 * ## Voice playback
 */

/**
 * Create a voice that plays an audio.
 *
 * `desc` voice parameters; must not be NULL. Not retained after the call.
 *
 * `return` the voice handle, or an id of `BXR_VOICE_INVALID_ID` if the audio
 * is invalid, no slot is free (`BXR_AUDIO_MAX_VOICES`), or the stream cannot
 * be created.
 *
 * **Note**: the caller must free the voice with `bxr_voice_destroy()`, unless
 * `play_once` is set, in which case it is freed automatically once finished.
 */
BXR_API bxr_voice_t bxr_voice_create(const bxr_voice_desc_t *desc);

/**
 * Set the volume of a voice.
 *
 * `voice` the voice to change.
 * `volume` gain clamped to [0.0, 1.0].
 */
BXR_API void bxr_voice_set_volume(bxr_voice_t voice, float volume);

/**
 * Get the volume of a voice.
 *
 * `voice` the voice to query.
 *
 * `return` the volume in [0.0, 1.0].
 */
BXR_API float bxr_voice_get_volume(bxr_voice_t voice);

/**
 * Start or resume playback from the current position.
 *
 * `voice` the voice to play.
 */
BXR_API void bxr_voice_play(bxr_voice_t voice);

/**
 * Pause playback, keeping the current position.
 *
 * `voice` the voice to pause.
 */
BXR_API void bxr_voice_pause(bxr_voice_t voice);

/**
 * Check whether a voice is not playing.
 *
 * `voice` the voice to query.
 *
 * `return` true if the voice is paused or stopped, false if it is playing.
 */
BXR_API bool bxr_voice_is_paused(bxr_voice_t voice);

/**
 * Stop playback and rewind to the start.
 *
 * `voice` the voice to stop.
 */
BXR_API void bxr_voice_stop(bxr_voice_t voice);

/**
 * Get the playback position of a voice.
 *
 * `voice` the voice to query.
 *
 * `return` the position in milliseconds, or 0 on error.
 */
BXR_API Uint64 bxr_voice_get_position(bxr_voice_t voice);

/**
 * Seek a voice to a position.
 *
 * `voice` the voice to seek.
 * `position_ms` target position in milliseconds; must be less than the audio
 * length.
 *
 * `return` true on success, false if the position is out of range or the
 * voice is invalid.
 */
BXR_API bool bxr_voice_set_position(bxr_voice_t voice, Uint64 position_ms);

/**
 * Set the playback speed of a voice, which also shifts its pitch.
 *
 * `voice` the voice to change.
 * `pitch` frequency ratio clamped to [0.01, 100.0]; 1.0 is normal speed.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool bxr_voice_set_pitch(bxr_voice_t voice, float pitch);

/**
 * Get the pitch of a voice.
 *
 * `voice` the voice to query.
 *
 * `return` the frequency ratio; 1.0 is normal speed.
 */
BXR_API float bxr_voice_get_pitch(bxr_voice_t voice);

/**
 * Set the stereo balance of a voice.
 *
 * Only mono and stereo sources can be panned. Sources with more channels play
 * with their own layout.
 *
 * `voice` the voice to change.
 * `pan` balance clamped to [-1.0, 1.0]: -1.0 left, 0.0 center, 1.0 right.
 *
 * `return` true on success, false on failure.
 *
 * **NOTE:** Only mono and stereo sources can be panned.
 */
BXR_API bool bxr_voice_set_pan(bxr_voice_t voice, float pan);

/**
 * Get the stereo balance of a voice.
 *
 * `voice` the voice to query.
 *
 * `return` the pan in [-1.0, 1.0].
 */
BXR_API float bxr_voice_get_pan(bxr_voice_t voice);

/**
 * Stop a voice and free it, removing it from all its tags.
 *
 * `voice` the voice to destroy.
 */
BXR_API void bxr_voice_destroy(bxr_voice_t voice);

/**
 * ## Tags
 */

/**
 * Add a voice to a tag.
 *
 * `tag_name` the tag; must not be NULL. The string is copied.
 * `voice` the voice to tag.
 *
 * `return` true on success, false on failure.
 *
 * FIXME: if `bxr_multimap_add()` fails, the tag stays in the voice's own list,
 * leaving both out of sync. Roll back the voice entry on failure.
 */
BXR_API bool bxr_audio_tag_add(const char *tag_name, bxr_voice_t voice);

/**
 * Remove a voice from a tag.
 *
 * `tag_name` the tag; must not be NULL.
 * `voice` the voice to untag.
 *
 * `return` true on success or if the voice has no tags, false if the voice
 * was not in the tag.
 *
 * FIXME: returns true when the voice has no tags at all but false when it has
 * other tags; the result should be consistent in both cases.
 */
BXR_API bool bxr_audio_tag_remove(const char *tag_name, bxr_voice_t voice);

/**
 * ## Group playback
 */

/**
 * Set the volume of every voice in a tag.
 *
 * `tag_name` the tag.
 * `volume` gain clamped to [0.0, 1.0].
 */
BXR_API void bxr_audio_tag_set_volume(const char *tag_name, float volume);

/**
 * Start or resume every voice in a tag.
 *
 * `tag_name` the tag.
 */
BXR_API void bxr_audio_tag_play(const char *tag_name);

/**
 * Pause every voice in a tag.
 *
 * `tag_name` the tag.
 */
BXR_API void bxr_audio_tag_pause(const char *tag_name);

/**
 * Stop and rewind every voice in a tag.
 *
 * `tag_name` the tag.
 */
BXR_API void bxr_audio_tag_stop(const char *tag_name);

#endif // BXR_AUDIO_H_
