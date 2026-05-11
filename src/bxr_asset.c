#include <physfs.h>

#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_mem.h"

#include "bxr_asset.h"

static Uint32 initialized_     = 0;
static bxr_context_t *context_ = NULL;

void
bxr_asset_setup(bxr_context_t *context)
{
  SDL_assert(initialized_ == 0);
  SDL_assert(context != NULL);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;

  if (!PHYSFS_init(NULL)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to init PhysFS (error: %s)",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return;
  }

  // PHYSFS get write directory (system specific)
  //
  // * - **Linux:** `~/.local/share/<config->name>/`
  // * - **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
  // * - **macOS:** `~/Library/Application Support/<config->name>/`
  //
  const char *write_dir
      = PHYSFS_getPrefDir(BXR_ORGANIZATION_NAME, context->config.name);
  if (!PHYSFS_setWriteDir(write_dir)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to set write dir to %s: (error: %s)",
                write_dir,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Write directory: %s", write_dir);

  // Mounted as prepend (higher priority than read-only data dir)
  if (!PHYSFS_mount(write_dir, NULL, 0)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to mount write dir %s: (error: %s)",
                write_dir,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return;
  }
}

void
bxr_asset_shutdown(void)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);

  PHYSFS_deinit();

  initialized_ = 0;
  context_     = NULL;
}

bool
bxr_asset_mount(const char *path)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(path);

  // Mounted as append (lower priority than write dir)
  if (!PHYSFS_mount(path, NULL, 1)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to mount %s: (error: %s)",
                path,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    // TODO set error
    // bxr_set_error(BXR_ERROR_ASSET_MOUNT);
    return false;
  }
  return true;
}

bool
bxr_asset_unmount(const char *path)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(path);

  if (!PHYSFS_unmount(path)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to unmount %s: (error: %s)",
                path,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    // TODO set error
    // bxr_set_error(BXR_ERROR_ASSET_UNMOUNT);
    return false;
  }
  return true;
}

bool
bxr_asset_exists(const char *path)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(path);

  return PHYSFS_exists(path) != 0;
}

bool
bxr_asset_is_directory(const char *path)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
  SDL_assert(path);

  PHYSFS_Stat statbuf = { 0 };
  if (!PHYSFS_stat(path, &statbuf)) {
    return (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK);
  }

  return false;
}

Uint8 *
bxr_asset_read(const char *path, size_t *length)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
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

  if (length) {
    *length = len;
  }

  if (len == 0) {
    SDL_LogInfo(
        SDL_LOG_CATEGORY_APPLICATION, "Reading an empty file: %s", path);
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

  return buffer;
}

bool
bxr_asset_write(const char *path, const void *data, size_t length, bool append)
{
  SDL_assert(initialized_ == BXR_INIT_COOKIE);
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
