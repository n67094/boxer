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
 * [Window]
 * title=My Game
 * width=800
 * height=600
 *
 * [Player]
 * name=nsix
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef ENGINE_INI_H_
#define ENGINE_INI_H_

#include <SDL3/SDL.h>

#include "engine_config.h"

#define ENGINE_INI_MAX_ROW_SIZE 512
#define ENGINE_INI_GROW_FACTOR 2

typedef struct engine_ini_s engine_ini_t;

/**
 * @brief Create a new, empty INI structure.
 *
 * @return A pointer to a newly allocated engine_ini_t structure, or NULL if
 * there was an error allocating memory.
 *
 * @note The caller is responsible for freeing the returned engine_ini_t
 */
engine_ini_t *engine_ini_make(void);

/**
 * @brief Free the memory associated with an engine_ini_t structure.
 *
 * @param ini A pointer to the engine_ini_t structure to free. If ini is NULL,
 * this function does nothing.
 */
void engine_ini_destroy(engine_ini_t *ini);

const char *engine_ini_get_filename(engine_ini_t *ini);

/**
 * @breif Read an INI file and return a structured representation of its
 * contents.
 *
 * @param filename The path to the INI file to read. (This will be read from the
 * PHYSFS filesystem configured).
 *
 * @return A pointer to an engine_ini_t structure containing the parsed INI
 * data, or NULL if there was an error reading the file.
 *
 * @note The caller is responsible for freeing the returned engine_ini_t
 */
engine_ini_t *engine_inir(const char *filename);

/**
 * @brief Retrieve a string value from the INI structure.
 *
 * @param ini The INI structure to query.
 * @param section The section name to look for.
 * @param key The key name to look for within the specified section.
 * @param default_value The default value to return if the section or key is not
 * found or if the value is not a string.
 *
 * @return The string value associated with the specified section and key, or
 * default_value if the section/key is not found or if the value is not a
 * string.
 */
const char *engine_inir_str(engine_ini_t *ini,
                            const char *section,
                            const char *key,
                            const char *default_value);

/**
 * @brief Retrieve a numeric value from the INI structure.
 *
 * @param ini The INI structure to query.
 * @param section The section name to look for.
 * @param key The key name to look for within the specified section.
 * @param default_value The default value to return if the section or key is not
 * found or if the value is not a number.
 *
 * @return The numeric value associated with the specified section and key, or
 * default_value if the section/key is not found or if the value is not a
 * number.
 */
float engine_inir_number(engine_ini_t *ini,
                         const char *section,
                         const char *key,
                         float default_value);

/**
 * @brief Set a string value in the INI structure.
 *
 * @param ini The INI structure to modify.
 * @param section The section name to set the value in. If the section does not
 * exist, it will be created.
 * @param key The key name to set the value for. If the key does not exist
 * within the specified section, it will be created.
 * @param value The string value to set for the specified section and key.
 */
void engine_iniw_str(engine_ini_t *ini,
                     const char *section,
                     const char *key,
                     const char *value);

/**
 * @brief Set a numeric value in the INI structure.
 *
 * @param ini The INI structure to modify.
 * @param section The section name to set the value in. If the section does not
 * exist, it will be created.
 * @param key The key name to set the value for. If the key does not exist
 * within the specified section, it will be created.
 * @param value The numeric value to set for the specified section and key.
 */
void engine_iniw_number(engine_ini_t *ini,
                        const char *section,
                        const char *key,
                        float value);

/**
 * @brief Write an INI file from a structured representation.
 *
 * @param ini The INI structure to write to file.
 * @param path The path to the INI file to write. This will be written to the
 * PHYSFS filesystem configured.
 * @param append Whether to append to the file (if true) or overwrite it (if
 * false).
 *
 * @return true if the INI file was successfully written, false otherwise.
 */
bool engine_iniw(engine_ini_t *ini, const char *path, bool append);

#endif // ENGINE_INI_H_
