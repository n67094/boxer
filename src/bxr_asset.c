#include <physfs.h>

#include "bxr_assert.h"
#include "bxr_asset.h"
#include "bxr_defs.h"
#include "bxr_log.h"
#include "bxr_mem.h"

static Uint32 initialized_     = 0;
static bxr_context_t *context_ = NULL;

void
bxr_asset_setup(bxr_context_t *context)
{
  BXR_ASSERT(initialized_ == 0);
  BXR_ASSERT(context != NULL);

  initialized_ = BXR_INIT_COOKIE;
  context_     = context;

  if (!PHYSFS_init(NULL)) {
    BXR_LOG_ERROR("Failed to init PhysFS (error: %s)",
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
    BXR_LOG_ERROR("Failed to set write dir to %s: (error: %s)",
                  write_dir,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return;
  }

  // Mounted as prepend (higher priority than read-only data dir)
  if (!PHYSFS_mount(write_dir, NULL, 0)) {
    BXR_LOG_ERROR("Failed to mount write dir %s: (error: %s)",
                  write_dir,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return;
  }

  BXR_LOG_INFO("Write directory: %s", write_dir);
}

void
bxr_asset_teardown(void)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);

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
    BXR_LOG_ERROR("Failed to mount %s: (error: %s)",
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
    BXR_LOG_ERROR("Failed to unmount %s: (error: %s)",
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
bxr_asset_is_directory(const char *path)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  PHYSFS_Stat statbuf = { 0 };
  if (!PHYSFS_stat(path, &statbuf)) {
    return (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK);
  }

  return false;
}

Uint8 *
bxr_asset_read(const char *path, size_t *length)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  if (!PHYSFS_exists(path)) {
    BXR_LOG_ERROR("File not found: %s", path);
    return NULL;
  }

  PHYSFS_File *file = PHYSFS_openRead(path);
  if (!file) {
    BXR_LOG_ERROR("Failed to open file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return NULL;
  }

  size_t len = PHYSFS_fileLength(file);

  if (length) {
    *length = len;
  }

  if (len == 0) {
    BXR_LOG_ERROR("Reading an empty file: %s", path);
    return NULL;
  }

  Uint8 *buffer = NULL;
  BXR_ALLOC(buffer, len);

  Sint64 read_size = PHYSFS_readBytes(file, buffer, len);
  if (read_size != len) {
    BXR_LOG_ERROR("Failed to read file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_close(file);
    return NULL;
  }

  PHYSFS_close(file);

  return buffer;
}

bool
bxr_asset_write(const char *path, const void *data, size_t length, bool append)
{
  BXR_ASSERT(initialized_ == BXR_INIT_COOKIE);
  BXR_ASSERT(path);

  PHYSFS_File *file = NULL;
  if (append) {
    file = PHYSFS_openAppend(path);
  } else {
    file = PHYSFS_openWrite(path);
  }

  if (!file) {
    BXR_LOG_ERROR("Failed to open file for writing: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return false;
  }

  if (!data || length == 0) {
    return true;
  }

  if (PHYSFS_writeBytes(file, data, length) != (Sint64)length) {
    BXR_LOG_ERROR("Failed to write to file: %s (error: %s)",
                  path,
                  PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    PHYSFS_close(file);
    return false;
  }

  PHYSFS_close(file);

  return true;
}
