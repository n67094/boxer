/**
 * File `bxr_io.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # IO Utilities for Reading and Writing Files
 *
 * File are `read` from `BXR_DATA_DIR` first then the system preferred path.
 *
 * Depending on the operating system, the `system preferred path` is as follows:
 *
 * - **Linux:** `~/.local/share/<config->name>/`
 * - **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
 * - **macOS:** `~/Library/Application Support/<config->name>/`
 */

#ifndef BXR_IO_H_
#define BXR_IO_H_

#include <SDL3/SDL.h>

#include <physfs.h>

#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_mem.h"

/**
 * Read a file from the given path and return its contents.
 *
 * `path` is the path to the file to read, relative to the data directory.
 *
 * `length` is an optional output parameter to store the length of the returned
 * data.
 *
 * `return` the contents of the file or NULL if an error occurred. Use
 * `bxr_error_get` to get more information about the error.
 *
 * The caller is responsible for freeing the returned data using `SDL_free` when
 * it is no longer needed.
 */
BXR_INLINE Uint8 *
bxr_io_read(const char *path, size_t *length)
{
  SDL_assert(path);

  if (!PHYSFS_exists(path)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File not found: %s", path);
    bxr_error_set(BXR_ERROR_FILE_NOT_FOUND);
    return NULL;
  }

  PHYSFS_File *file = PHYSFS_openRead(path);
  if (!file) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to open file: %s (error: %s)",
                 path,
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    bxr_error_set(BXR_ERROR_IO_READ);
    return NULL;
  }

  size_t len = PHYSFS_fileLength(file);
  if (len == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File is empty: %s", path);
    PHYSFS_close(file);
    bxr_error_set(BXR_ERROR_IO_READ);
    return NULL;
  }

  Uint8 *buffer = NULL;
  BXR_ALLOC(buffer, len);

  Sint64 read_size = PHYSFS_readBytes(file, buffer, len);
  if (read_size != len) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to read file: %s (error: %s)",
                 path,
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_close(file);
    bxr_error_set(BXR_ERROR_IO_READ);
    return NULL;
  }

  PHYSFS_close(file);

  if (length) {
    *length = len;
  }

  return buffer;
}

/**
 * Write data to a file at the given path.
 *
 * `path` is the path to the file to write, relative to the data directory.
 *
 * `data` is a pointer to the data to write.
 *
 * `length` is the length of the data in bytes.
 *
 * `append` is a boolean flag indicating whether to append to the file (if true)
 * or overwrite it (if false).
 *
 * `return` true if the write operation was successful, false otherwise. Use
 * `bxr_error_get` to get more information about the error if the write
 */
BXR_INLINE bool
bxr_io_write(const char *path, const void *data, size_t length, bool append)
{
  SDL_assert(path);

  PHYSFS_File *file = NULL;
  if (append) {
    file = PHYSFS_openAppend(path);
  } else {
    file = PHYSFS_openWrite(path);
  }

  if (!file) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to open file for writing: %s (error: %s)",
                 path,
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    bxr_error_set(BXR_ERROR_IO_WRITE);
    return false;
  }

  if (!data || length == 0) {
    return true;
  }

  if (PHYSFS_writeBytes(file, data, length) != (Sint64)length) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to write to file: %s (error: %s)",
                 path,
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_close(file);
    bxr_error_set(BXR_ERROR_IO_WRITE);
    return false;
  }

  PHYSFS_close(file);

  return true;
}

#endif // BXR_IO_H_
