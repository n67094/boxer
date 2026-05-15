/**
 * File `bxr_ini.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # INI File Reader and Writer
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

#include "bxr_defs.h"

typedef struct bxr_ini_reader_s bxr_ini_reader_t;

/**
 * ## Reader API
 */

/**
 * Load an INI file from the given data and return a reader for it.
 *
 * `data` is a pointer to the INI data to load.
 *
 * `length` is the length of the INI data in bytes.
 *
 * `return` a reader for the loaded INI file, or NULL if an error occurred. Use
 * `bxr_error_get` to get more information about the error.
 *
 * The caller is responsible for destroying the returned reader using
 * `bxr_ini_destroy_reader` when it is no longer needed.
 */
BXR_API bxr_ini_reader_t *bxr_ini_create_reader(const Uint8 *data,
                                                size_t length);

/**
 * Destroy an INI reader and free its resources.
 *
 * `ini` is the INI reader to destroy.
 */
BXR_API void bxr_ini_destroy_reader(bxr_ini_reader_t *ini);

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
BXR_API const char *
bxr_ini_read_str(bxr_ini_reader_t *ini, const char *section, const char *key);

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
BXR_API float bxr_ini_read_number(bxr_ini_reader_t *ini,
                                  const char *section,
                                  const char *key);

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
BXR_API const char *bxr_ini_read_str_or_else(bxr_ini_reader_t *ini,
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
BXR_API float bxr_ini_read_number_or_else(bxr_ini_reader_t *ini,
                                          const char *section,
                                          const char *key,
                                          float default_value);

/**
 * ## Writer API
 */

typedef struct bxr_ini_writer_s bxr_ini_writer_t;

/**
 * Create a new INI writer.
 *
 * `return` a new INI writer, or NULL an error occurred. Use `bxr_error_get` to
 * get more information about the error.
 *
 * The caller is responsible for destroying the returned writer using
 * `bxr_ini_destroy_writer` when it is no longer needed.
 */
BXR_API bxr_ini_writer_t *bxr_ini_create_writer(void);

/**
 * Destroy an INI writer and free its resources.
 *
 * `ini` is the INI writer to destroy.
 */
BXR_API void bxr_ini_destroy_writer(bxr_ini_writer_t *ini);

/**
 * Begin a new section in the INI writer.
 *
 * `ini` is the INI writer to use.
 *
 * `section` is the name of the section to begin.
 *
 * `return` true if the section was successfully begun, or false if an error
 * occurred. Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool bxr_ini_writer_section_begin(bxr_ini_writer_t *ini,
                                          const char *section);

/**
 * End the current section in the INI writer.
 *
 * `ini` is the INI writer to use.
 *
 * `return` true if the section was successfully ended, or false if an error
 * occurred. Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool bxr_ini_writer_section_end(bxr_ini_writer_t *ini);

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
 * error occurred. Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool
bxr_ini_write_str(bxr_ini_writer_t *ini, const char *key, const char *value);

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
 * error occurred. Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool
bxr_ini_write_number(bxr_ini_writer_t *ini, const char *key, float number);

/**
 * Get the INI data from the writer.
 *
 * `ini` is the INI writer to get the data from.
 *
 * `length` is an optional output parameter to store the length of the returned
 * data in bytes.
 *
 * *
 * `return` a pointer to the Ini data written by the writer. The caller should
 * not modify or free this pointer, as it is owned by the JSON writer.
 */
BXR_API const Uint8 *bxr_ini_writer_get_data(const bxr_ini_writer_t *ini,
                                             size_t *length);

#endif // BXR_INI_H_
