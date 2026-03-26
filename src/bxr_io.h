#ifndef BXR_IO_H_
#define BXR_IO_H_

#include <SDL3/SDL.h>

#include <physfs.h>

#include "bxr_config.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_mem.h"

BXR_INLINE char *
bxr_io_read(const char *path, size_t *length)
{
  SDL_assert(path);

  if (!PHYSFS_exists(path)) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "File not found: %s", path);
    return NULL;
  }

  PHYSFS_File *file = PHYSFS_openRead(path);
  if (!file) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to open file: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    bxr_set_error(BXR_ERROR_IO_READ);
    return NULL;
  }

  size_t len = PHYSFS_fileLength(file);
  if (len == 0) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "File is empty: %s", path);
    PHYSFS_close(file);
    bxr_set_error(BXR_ERROR_IO_READ);
    return NULL;
  }

  if (length) {
    *length = len;
  }

  char *buffer = NULL;
  BXR_ALLOC(buffer, *length);

  Sint64 read_size = PHYSFS_readBytes(file, buffer, len);
  if (read_size != (Sint64)*length) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to read file: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    PHYSFS_close(file);
    bxr_set_error(BXR_ERROR_IO_READ);
    return NULL;
  }

  PHYSFS_close(file);

  buffer[*length - 1] = '\0';

  return buffer;
}

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
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to open file for writing: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    bxr_set_error(BXR_ERROR_IO_WRITE);
    return false;
  }

  if (!data || length == 0) {
    return true;
  }

  if (PHYSFS_writeBytes(file, data, length) != (Sint64)length) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to write to file: %s (error code: %d)",
                path,
                PHYSFS_getLastErrorCode());
    PHYSFS_close(file);
    bxr_set_error(BXR_ERROR_IO_WRITE);
    return false;
  }

  PHYSFS_close(file);

  return true;
}

#endif // BXR_IO_H_
