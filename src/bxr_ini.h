/**
 * file `bxr_ini.h`
 *
 * copyright Copyright (c) 2026 nsix. All rights reserved.
 *
 * # INI file reader (inir) and writer (iniw).
 *
 * Support strings and numbers (float, int, etc).
 *
 * File extension doesn't matter, but recommended to use `.ini`.
 *
 * Example INI file:
 *
 * ```
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
 * ```
 */

#ifndef BXR_INI_H_
#define BXR_INI_H_

#include <SDL3/SDL.h>

#define BXR_INI_MAX_ROW_SIZE 256
#define BXR_INI_DEFAULT_DATA_CAPACITY 512

typedef struct bxr_inir_s bxr_inir_t;

/**
 * ## Reader API
 */

bxr_inir_t *bxr_inir(const char *path);

void bxr_inir_destroy(bxr_inir_t *ini);

const char *bxr_inir_str(bxr_inir_t *ini, const char *section, const char *key);

float bxr_inir_number(bxr_inir_t *ini, const char *section, const char *key);

const char *bxr_inir_str_or_else(bxr_inir_t *ini,
                                 const char *section,
                                 const char *key,
                                 const char *default_value);

float bxr_inir_number_or_else(bxr_inir_t *ini,
                              const char *section,
                              const char *key,
                              float default_value);

/**
 * ## Writer API
 */

typedef struct bxr_iniw_s bxr_iniw_t;

bxr_iniw_t *bxr_iniw(void);

void bxr_iniw_destroy(bxr_iniw_t *ini);

bool bxr_iniw_section_begin(bxr_iniw_t *ini, const char *section);

bool bxr_iniw_section_end(bxr_iniw_t *ini);

bool bxr_iniw_key_str(bxr_iniw_t *ini, const char *key, const char *value);

bool bxr_iniw_key_number(bxr_iniw_t *ini, const char *key, float number);

bool bxr_iniw_save(bxr_iniw_t *ini, const char *path);

#endif // BXR_INI_H_
