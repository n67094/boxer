/**
 * @file bxr_atom.h
 *
 * @brief String atom table.
 *
 * @details The atom table allows for efficient storage and retrieval of
 * strings. Any added strings are unique and saved for the application lifetime.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ATOM_H_
#define BXR_ATOM_H_

#include <SDL3/SDL.h>

/**
 * @brief Add a string to the table.
 *
 * @param str The null terminated string to add.
 *
 * @return a pointer to the unique string.
 */
const char *bxr_atom_make(const char *str);

/**
 * @brief Add a string from memory to the table.
 *
 * @param str The string to add from memory.
 * @param len The lenght of the string to add.
 *
 * @return a pointer to the unique string.
 */
const char *bxr_atom_make_mem(const char *str, size_t len);

/**
 * @brief return the length of the atom.
 */
size_t bxr_atom_length(const char *str);

#endif // BXR_ATOM_H_
