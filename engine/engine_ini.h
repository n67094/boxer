/**
 * @file engine_ini.h
 *
 * @brief INI file reader (inir) and writer (iniw).
 *
 * @details Support strings and numbers (float, int, etc).
 *
 * File extension doesn't matter, but recommended to use ".ini".
 *
 * Example INI file:
 *
 * [Window] ; comment
 * title="Test"
 * width=800
 * height=600
 *
 * ; comment
 *
 * [Player]
 * name="nsix\ttest" ; Please note that the name contains a tab character
 * test= ; this will be ignored
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_INI_H_
#define ENGINE_INI_H_

#include <SDL3/SDL.h>

#define ENGINE_INI_MAX_ROW_SIZE 256
#define ENGINE_INI_DEFAULT_DATA_CAPACITY 512

typedef struct engine_inir_s engine_inir_t;

engine_inir_t *engine_inir(const char *path);
void engine_inir_destroy(engine_inir_t *ini);

const char *
engine_inir_str(engine_inir_t *ini, const char *section, const char *key);
float
engine_inir_number(engine_inir_t *ini, const char *section, const char *key);

const char *engine_inir_str_or_else(engine_inir_t *ini,
                                    const char *section,
                                    const char *key,
                                    const char *default_value);
float engine_inir_number_or_else(engine_inir_t *ini,
                                 const char *section,
                                 const char *key,
                                 float default_value);

// ----------------------------------------------------------------------------

typedef struct engine_iniw_s engine_iniw_t;

engine_iniw_t *engine_iniw(void);
void engine_iniw_destroy(engine_iniw_t *ini);

bool engine_iniw_section_begin(engine_iniw_t *ini, const char *section);
bool engine_iniw_section_end(engine_iniw_t *ini);

bool
engine_iniw_key_str(engine_iniw_t *ini, const char *key, const char *value);
bool engine_iniw_key_number(engine_iniw_t *ini, const char *key, float number);
bool engine_iniw_save(engine_iniw_t *ini, const char *path);

#endif // ENGINE_INI_H_
