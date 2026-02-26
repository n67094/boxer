#include <SDL3/SDL.h>
#include <SDL3/SDL_mixer.h>

#include "engine_audio.h"
#include "engine_config.h"
#include "engine_context.h"
#include "engine_pool.h"

typedef struct _engine_audio_s
{
  SDL_AudioDeviceID device;
  float master_volume;
  float master_pan;
  float master_pitch;
} _engine_audio_t;

typedef struct _engine_sound_s
{
  SDL_AudioStream *stream;
  SDL_AudioSpec spec;
  engine_sound_desc_t desc;
} _engine_sound_t;

static engine_pool_t *_sound_pool;
static _engine_sound_t *_sounds;

static Uint32 _initialized = 0;
static engine_context_t *_context;

void engine_audio_setup(void);

void engine_audio_update(void);

void engine_audio_shutdown(void);
