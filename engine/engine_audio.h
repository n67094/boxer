/**
 * @file engine_audio.h
 *
 * @brief Audio system for playing sound effects and music.
 *
 * @details This module provides functions for loading, playing, pausing,
 * resuming, and stopping audio. It also supports audio tags for grouping sounds
 * and controlling them together, for example you can tag all music tracks with
 * "music" and then control their volume or playback state together.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_AUDIO_H_
#define ENGINE_AUDIO_H_

#include <SDL3/SDL.h>

typedef enum
{
  ENGINE_AUDIO_STATE_NONE,
  ENGINE_AUDIO_STATE_PLAYING,
  ENGINE_AUDIO_STATE_PAUSED,
} engine_audio_state_e;

typedef struct engine_sound_s
{
  Uint32 id;
} engine_sound_t;

typedef struct engine_sound_desc_s
{
  engine_audio_state_e state;
  bool looping;
  float volume;
  float pan;
  float pitch;
  float start_time_ms;
  float fade_in_time_ms;
  float fade_out_time_ms;

  void (*callback)(engine_sound_t sound, void *user_data);
  void *user_data;
} engine_sound_desc_t;

// Internal API - Audio
// ----------------------------------------------------------------------------

void engine_audio_setup(void);
void engine_audio_update(void);
void engine_audio_shutdown(void);

// Public API - Audio (master)
// ----------------------------------------------------------------------------

void engine_audio_set_volume(float volume);
float engine_audio_get_volume(void);

void engine_audio_set_pan(float pan);
float engine_audio_get_pan(void);

void engine_audio_set_pitch(float pitch);
float engine_audio_get_pitch(void);

// TODO fade in/out ?

void engine_audio_set_play(void);
void engine_audio_set_pause(void);
void engine_audio_set_resume(void);
void engine_audio_set_stop(void);

// Public API - Audio Tags
// ----------------------------------------------------------------------------

void engine_audio_tag_play(const char *tag);
void engine_audio_tag_pause(const char *tag);
void engine_audio_tag_resume(const char *tag);
void engine_audio_tag_stop(const char *tag);

// TODO fade in/out ?

void engine_audio_tag_set_volume(const char *tag, float volume);

// Public API - Sound
// ----------------------------------------------------------------------------

engine_sound_t engine_sound_load(const char *path);
engine_sound_t engine_sound_load_mem();

void engine_sound_destroy(engine_sound_t audio);

int engine_sound_get_sample_rate(engine_sound_t sound);
int engine_sound_get_sample_count(engine_sound_t sound);
int engine_sound_get_channels(engine_sound_t sound);

// add tag
// Remove tag
// Get tags

bool engine_sound_is_playing(engine_sound_t sound);

void engine_sound_set_looping(engine_sound_t sound, bool looping);
bool engine_sound_is_looping(engine_sound_t sound);

void engine_sound_set_volume(engine_sound_t sound, float volume);
float engine_sound_get_volume(engine_sound_t sound);

void engine_sound_set_pan(engine_sound_t sound, float pan);
float engine_sound_get_pan(engine_sound_t sound);

void engine_sound_set_pitch(engine_sound_t sound, float pitch);
float engine_sound_get_pitch(engine_sound_t sound);

void engine_sound_set_start_time(engine_sound_t sound, float start_time_ms);
float engine_sound_get_start_time(engine_sound_t sound);

void engine_sound_set_fade_in_time(engine_sound_t sound, float fade_in_time_ms);
float engine_sound_get_fade_in_time(engine_sound_t sound);

void engine_sound_set_fade_out_time(engine_sound_t sound,
                                    float fade_out_time_ms);
float engine_sound_get_fade_out_time(engine_sound_t sound);

void engine_sound_set_callback(engine_sound_t sound,
                               void (*callback)(engine_sound_t sound,
                                                void *user_data),
                               void *user_data);

void engine_sound_play(engine_sound_t sound, engine_sound_desc_t *desc);
void engine_sound_pause(engine_sound_t sound);
void engine_sound_resume(engine_sound_t sound);
void engine_sound_stop(engine_sound_t sound);

void engine_sound_switch(engine_sound_t new_sound,
                         float fade_out_time_ms,
                         float fade_in_time_ms);

void engine_sound_crossfade(engine_sound_t new_sound, float crossfade_time_ms);

#endif // ENGINE_AUDIO_H_
