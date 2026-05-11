#include "bxr_io.h"

void
bxr_io_setup(void)
{
}

void
bxr_io_shutdown(void)
{
}

Uint8 *
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

bool
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
