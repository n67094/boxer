/**
 * File `bxr_ini.h`.
 *
 * INI reader and writer.
 *
 * Supported types:
 *
 * - string
 * - integer
 * - float
 * - boolean (0 or 1)
 *
 * File extension doesn't matter, but `.ini` is recommended.
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
 *
 * Copyright (c) 2026 nsix. All rights reserved.
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
 * Parse INI data and return a reader for it.
 *
 * Malformed lines are skipped silently, so parsing itself never fails.
 *
 * `data` INI text. Must not be `NULL`. Remains owned by the caller and may be
 * freed once the call returns.
 * `length` size of `data` in bytes.
 *
 * `return` the reader, or `NULL` on failure.
 *
 * **Note**: the caller must free the result with `bxr_ini_destroy_reader()`.
 */
BXR_API bxr_ini_reader_t *bxr_ini_create_reader(const Uint8 *data,
                                                size_t length);

/**
 * Destroy a reader and free its resources.
 *
 * `ini` reader to destroy.
 *
 * **NOTE:** String returned by the `bxr_ini_read_*()` functions become invalid.
 */
BXR_API void bxr_ini_destroy_reader(bxr_ini_reader_t *ini);

/**
 * Get the string value of a key.
 *
 * If the key appears several times in the section, the first one wins.
 *
 * `ini` reader to query. Must not be `NULL`.
 * `section` section name, case-insensitive. Must not be `NULL`.
 * `key` key name, case-insensitive. Must not be `NULL`.
 *
 * `return` the unquoted and unescaped value, or `NULL` if not found.
 *
 * **Note**: the result is owned by the reader and stays valid until
 * `bxr_ini_destroy_reader()`.
 */
BXR_API const char *
bxr_ini_read_str(bxr_ini_reader_t *ini, const char *section, const char *key);

/**
 * Get the value of a key parsed as a number.
 *
 * `ini` reader to query. Must not be `NULL`.
 * `section` section name, case-insensitive. Must not be `NULL`.
 * `key` key name, case-insensitive. Must not be `NULL`.
 *
 * `return` the parsed value, or `0.0f` if not found.
 */
BXR_API float bxr_ini_read_number(bxr_ini_reader_t *ini,
                                  const char *section,
                                  const char *key);

/**
 * Get the string value of a key, or a fallback if it is missing.
 *
 * `ini` reader to query. Must not be `NULL`.
 * `section` section name, case-insensitive. Must not be `NULL`.
 * `key` key name, case-insensitive. Must not be `NULL`.
 * `default_value` returned as is when the key is not found. May be `NULL`.
 *
 * `return` the value (owned by the reader, see `bxr_ini_read_str()`), or
 * `default_value` if not found.
 */
BXR_API const char *bxr_ini_read_str_or_else(bxr_ini_reader_t *ini,
                                             const char *section,
                                             const char *key,
                                             const char *default_value);

/**
 * Get the value of a key parsed as a number, or a fallback if it is missing.
 *
 * `ini` reader to query. Must not be `NULL`.
 * `section` section name, case-insensitive. Must not be `NULL`.
 * `key` key name, case-insensitive. Must not be `NULL`.
 * `default_value` returned when the key is not found.
 *
 * `return` the parsed value, or `default_value` if not found.
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
 * Create an empty INI writer.
 *
 * `return` the writer, or `NULL` on failure.
 *
 * **Note**: the caller must free the result with `bxr_ini_destroy_writer()`.
 */
BXR_API bxr_ini_writer_t *bxr_ini_create_writer(void);

/**
 * Destroy a writer and free its resources.
 *
 * `ini` writer to destroy. `NULL` is allowed and does nothing.
 */
BXR_API void bxr_ini_destroy_writer(bxr_ini_writer_t *ini);

/**
 * Start a section by writing its `[section]` header.
 *
 * Sections cannot be nested: the previous one must be closed with
 * `bxr_ini_writer_section_end()` first.
 *
 * `ini` writer to use. Must not be `NULL`.
 * `section` section name, written as is. Must not be `NULL`.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool bxr_ini_writer_section_begin(bxr_ini_writer_t *ini,
                                          const char *section);

/**
 * Close the current section by writing an empty line.
 *
 * `ini` writer to use, with a section open. Must not be `NULL`.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool bxr_ini_writer_section_end(bxr_ini_writer_t *ini);

/**
 * Write a `key="value"` line in the current section.
 *
 * The value is quoted and escaped.
 *
 * `ini` writer to use, with a section open. Must not be `NULL`.
 * `key` key name, written as is. Must not be `NULL`.
 * `value` string value. Must not be `NULL`.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool
bxr_ini_write_str(bxr_ini_writer_t *ini, const char *key, const char *value);

/**
 * Write a `key=number` line in the current section.
 *
 * The number is formatted with `%g`, so only 6 significant digits are kept.
 *
 * `ini` writer to use, with a section open. Must not be `NULL`.
 * `key` key name, written as is. Must not be `NULL`.
 * `number` value to write.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool
bxr_ini_write_number(bxr_ini_writer_t *ini, const char *key, float number);

/**
 * Save the INI text written so far to a file.
 *
 * `ini` writer to read from. Must not be `NULL`.
 * `path` path to write to. Must not be `NULL`.
 *
 * `return` true on success, false on failure.
 */
BXR_API bool bxr_ini_writer_save(bxr_ini_writer_t *ini, const char *path);

/**
 * Get the INI text written so far.
 *
 * `ini` writer to read from. Must not be `NULL`.
 * `length` receives the size of the data in bytes, without the null
 * terminator. May be `NULL`.
 *
 * `return` the null-terminated INI text. Never `NULL`.
 *
 * **Note**: the result is owned by the writer. Do not modify or free it; it
 * becomes invalid after the next write or `bxr_ini_destroy_writer()`.
 */
BXR_API const Uint8 *bxr_ini_writer_get_data(const bxr_ini_writer_t *ini,
                                             size_t *length);

#endif // BXR_INI_H_
