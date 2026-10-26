#include <physfs.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_defs.h"
#include "bxr_error.h"
#include "bxr_log.h"
#include "bxr_mem.h"

static Uint32 initialized_     = 0;
static bxr_context_t *context_ = NULL;

bool
bxr_asset_setup(bxr_context_t *context)
{
  BXR_ASSERT(initialized_ == 0);
  BXR_ASSERT(context != NULL);

  if (!PHYSFS_init(NULL)) {
    bxr_set_error("Failed to init PhysFS (error: %s)",
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }

  // PHYSFS get write directory (system specific)
  //
  // * - **Linux:** `~/.local/share/<config->name>/`
  // * - **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
  // * - **macOS:** `~/Library/Application Support/<config->name>/`
  //
  const char *write_dir
      = PHYSFS_getPrefDir(BXR_ORGANIZATION_NAME, context->config.name);
  if (!write_dir) {
    bxr_set_error("Failed to get write dir: (error: %s)",
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_deinit();
    return false;
  }

  if (!PHYSFS_setWriteDir(write_dir)) {
    bxr_set_error("Failed to set write dir to %s: (error: %s)",
                  write_dir,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_deinit();
    return false;
  }

  // Mounted as prepend (higher priority than read-only data dir)
  if (!PHYSFS_mount(write_dir, NULL, 0)) {
    bxr_set_error("Failed to mount write dir %s: (error: %s)",
                  write_dir,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_deinit();
    return false;
  }

  BXR_LOG_INFO("Write directory: %s", write_dir);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;

  return true;
}

void
bxr_asset_teardown(void)
{
  // Also called after a failed or skipped setup
  if (initialized_ != BXR_INIT_COOKIE) {
    return;
  }

  PHYSFS_deinit();

  initialized_ = 0;
  context_     = NULL;
}

bool
bxr_asset_mount(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  // Mounted as append (lower priority than write dir)
  if (!PHYSFS_mount(path, NULL, 1)) {
    bxr_set_error("Failed to mount %s: (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }
  return true;
}

bool
bxr_asset_unmount(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  if (!PHYSFS_unmount(path)) {
    bxr_set_error("Failed to unmount %s: (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }
  return true;
}

bool
bxr_asset_exists(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  return PHYSFS_exists(path) != 0;
}

bool
bxr_asset_has_extension(const char *path, const char *extension)
{
  BXR_ASSERT(path);
  BXR_ASSERT(extension);

  const char *name = SDL_strrchr(path, '/');
  name             = name ? name + 1 : path;

  const char *dot = SDL_strrchr(name, '.');
  if (!dot || dot == name) {
    return false;
  }

  return SDL_strcasecmp(dot, extension) == 0;
}

bool
bxr_asset_is_directory(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  PHYSFS_Stat statbuf = { 0 };
  if (PHYSFS_stat(path, &statbuf)) {
    return (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY);
  }

  return false;
}

Uint8 *
bxr_asset_read(const char *path, size_t *length)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  if (!PHYSFS_exists(path)) {
    bxr_set_error("File not found: %s", path);
    return NULL;
  }

  PHYSFS_File *file = PHYSFS_openRead(path);
  if (!file) {
    bxr_set_error("Failed to open file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return NULL;
  }

  Sint64 len = PHYSFS_fileLength(file);
  if (len <= 0) {
    bxr_set_error("Empty file: %s", path);
    PHYSFS_close(file);
    if (length) {
      *length = 0;
    }
    return NULL;
  }

  Uint8 *buffer = NULL;
  buffer        = BXR_ALLOC(len);
  if (!buffer) {
    bxr_set_error("Failed to allocate memory for file: %s", path);
    PHYSFS_close(file);
    return NULL;
  }

  Sint64 read_size = PHYSFS_readBytes(file, buffer, len);
  if (read_size != len) {
    bxr_set_error("Failed to read file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    BXR_FREE(buffer);
    PHYSFS_close(file);
    return NULL;
  }

  if (!PHYSFS_close(file)) {
    bxr_set_error("Failed to close file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    BXR_FREE(buffer);
    return NULL;
  }

  if (length) {
    *length = (size_t)len;
  }

  return buffer;
}

bool
bxr_asset_write(const char *path, const void *data, size_t length, bool append)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  if (!data || length == 0) {
    return bxr_set_error("Nothing to write to file: %s", path);
  }

  PHYSFS_File *file = NULL;
  if (append) {
    file = PHYSFS_openAppend(path);
  } else {
    file = PHYSFS_openWrite(path);
  }

  if (!file) {
    bxr_set_error("Failed to open file for writing: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }

  if (PHYSFS_writeBytes(file, data, length) != (Sint64)length) {
    bxr_set_error("Failed to write to file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_close(file);
    return false;
  }

  if (!PHYSFS_close(file)) {
    bxr_set_error("Failed to close file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }

  return true;
}

bool
bxr_asset_delete(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  if (!PHYSFS_delete(path)) {
    bxr_set_error("Failed to delete file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }

  return true;
}
