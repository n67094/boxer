/**
 * File `bxr_ini.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # INI File Reader `inir` and Writer `iniw`
 *
 * Support strings and numbers. Booleans are not supported, but you can use `0`
 * and `1` to represent false and true respectively.
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

typedef struct bxr_inir_s bxr_inir_t;

/**
 * ## Reader API
 */

/**
 * Load an INI file from the given path and return a reader for it.
 *
 * `path` is the path to the INI file to load. (c.f `bxr_io_read` for path
 * resolution).
 *
 * `return` a reader for the loaded INI file, or NULL if the file could not be
 * loaded.
 */
bxr_inir_t *bxr_inir_make(const char *path);

/**
 * Destroy an INI reader and free its resources.
 *
 * `ini` is the INI reader to destroy.
 */
void bxr_inir_destroy(bxr_inir_t *ini);

/**
 * Get the string value associated with the given section and key.
 *
 * `ini` is the INI reader to query.
 *
 * `section` is the section name to look for.
 *
 * `key` is the key name to look for within the section.
 *
 * `return` the string value associated with the given section and key, or NULL
 * if the section or key was not found.
 */
const char *bxr_inir_str(bxr_inir_t *ini, const char *section, const char *key);

/**
 * Get the number value associated with the given section and key.
 *
 * `ini` is the INI reader to query.
 *
 * `section` is the section name to look for.
 *
 * `key` is the key name to look for within the section.
 *
 * `return` the number value associated with the given section and key, or 0.0
 * if the section or key was not found.
 */
float bxr_inir_number(bxr_inir_t *ini, const char *section, const char *key);

/**
 * Get the string value associated with the given section and key, or return a
 * default value if the section or key was not found.
 *
 * `ini` is the INI reader to query.
 *
 * `section` is the section name to look for.
 *
 * `key` is the key name to look for within the section.
 *
 * `default_value` is the default value to return if the section or key was not
 * found.
 *
 * `return` the string value associated with the given section and key, or
 * `default_value` if the section or key was not found.
 */
const char *bxr_inir_str_or_else(bxr_inir_t *ini,
                                 const char *section,
                                 const char *key,
                                 const char *default_value);

/**
 * Get the number value associated with the given section and key, or return a
 * default value if the section or key was not found.
 *
 * `ini` is the INI reader to query.
 *
 * `section` is the section name to look for.
 *
 * `key` is the key name to look for within the section.
 *
 * `default_value` is the default value to return if the section or key was not
 * found.
 *
 * `return` the number value associated with the given section and key, or
 * `default_value` if the section or key was not found.
 */
float bxr_inir_number_or_else(bxr_inir_t *ini,
                              const char *section,
                              const char *key,
                              float default_value);

/**
 * ## Writer API
 */

typedef struct bxr_iniw_s bxr_iniw_t;

/**
 * Create a new INI writer.
 *
 * `return` a new INI writer, or NULL if the writer could not be created.
 */
bxr_iniw_t *bxr_iniw_make(void);

/**
 * Destroy an INI writer and free its resources.
 *
 * `ini` is the INI writer to destroy.
 */
void bxr_iniw_destroy(bxr_iniw_t *ini);

/**
 * Begin a new section in the INI writer.
 *
 * `ini` is the INI writer to use.
 *
 * `section` is the name of the section to begin.
 *
 * `return` true if the section was successfully begun, or false if an error
 * occurred.
 */
bool bxr_iniw_section_begin(bxr_iniw_t *ini, const char *section);

/**
 * End the current section in the INI writer.
 *
 * `ini` is the INI writer to use.
 *
 * `return` true if the section was successfully ended, or false if an error
 * occurred.
 */
bool bxr_iniw_section_end(bxr_iniw_t *ini);

/**
 * Add a key-value pair with a string value to the current section in the INI
 * writer.
 *
 * `ini` is the INI writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `value` is the string value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_iniw_key_str(bxr_iniw_t *ini, const char *key, const char *value);

/**
 * Add a key-value pair with a number value to the current section in the INI
 * writer.
 *
 * `ini` is the INI writer to use.
 *
 * `key` is the name of the key to add.
 *
 * `number` is the number value to associate with the key.
 *
 * `return` true if the key-value pair was successfully added, or false if an
 * error occurred.
 */
bool bxr_iniw_key_number(bxr_iniw_t *ini, const char *key, float number);

/**
 * Save the INI data to a file at the given path.
 *
 * `ini` is the INI writer containing the data to save.
 *
 * `path` is the path to the file to save. (c.f `bxr_io_write` for path
 * resolution).
 *
 * `return` true if the INI data was successfully saved, or false if an error
 * occurred.
 */
bool bxr_iniw_save(bxr_iniw_t *ini, const char *path);

#endif // BXR_INI_H_
