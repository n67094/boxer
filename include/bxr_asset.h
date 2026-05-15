/**
 * File `bxr_asset.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Asset Management
 *
 * The asset system provides an interface for loading and managing game assets
 * such as images, sounds, and other data files. It uses a virtual file system
 * to allow mounting directories and archives (e.g. zip files).
 *
 * The asset system is built on top of the PhysicsFS library, which provides an
 * abstraction layer for file I/O and supports various archive formats:
 *
 * Currently supported archive types:
 *
 *    .ZIP (pkZip/WinZip/Info-ZIP compatible)
 *    .7Z (7zip archives)
 *    .ISO (ISO9660 files, CD-ROM images)
 *    .GRP (Build Engine groupfile archives)
 *    .PAK (Quake I/II archive format)
 *    .HOG (Descent I/II/III HOG file archives)
 *    .MVL (Descent II movielib archives)
 *    .WAD (DOOM engine archives)
 *    .BIN (Chasm: The Rift engine archives)
 *    .VDF (Gothic I/II engine archives)
 *    .SLB (Independence War archives)
 *    .POD (Terminal Velocity POD archives)
 *    .DAT (Resident Evil 3 rofs.dat archives)
 */

#ifndef BXR_ASSET_H_
#define BXR_ASSET_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"
#include "bxr_defs.h"

/**
 * ## Internal API
 */

/**
 * Initialize the asset subsystem. Must be called before using any other asset
 * functions.
 *
 * `context` is the bxr context to use for asset operations.
 */
BXR_API void bxr_asset_setup(bxr_context_t *context);

/**
 * Shutdown the asset subsystem.
 */
BXR_API void bxr_asset_shutdown(void);

/**
 * ## Public API
 */

/**
 * Mount a directory or archive (e.g. zip file) to the virtual file system.
 *
 * If this is a duplicate, the entry is not added again, even though the
 * function succeeds.
 *
 * If two archive are mounted with the same path they are merged together, if
 * there is a conflict file of the first mounted archive will be used.
 *
 * `path` directory or archive to mount (in platform-dependent
 * notation).
 *
 * `return` true if the mount operation was successful, false otherwise.
 * Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool bxr_asset_mount(const char *path);

/**
 * Unmount a directory or archive from the virtual file system.
 *
 * `path` directory or archive to unmount (in platform-dependent notation).
 *
 * `return` true if the unmount operation was successful, false otherwise.
 * Use `bxr_error_get` to get more information about the error.
 */
BXR_API bool bxr_asset_unmount(const char *path);

/**
 * Check if a file or directory exists at the given path.
 *
 * `path` is the path to the file or directory to check, relative to the data
 * directory.
 *
 * `return` true if the file or directory exists, false otherwise.
 */
BXR_API bool bxr_asset_exists(const char *path);

/**
 * Check if the given path is a directory.
 *
 * `path` is the path to check, relative to the data directory.
 *
 * `return` true if the path is a directory, false otherwise.
 */
BXR_API bool bxr_asset_is_directory(const char *path);

/**
 * Read a file from the given path and return its contents.
 *
 * `path` is the path to the file to read, relative to the mounted data
 * directories.
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
BXR_API Uint8 *bxr_asset_read(const char *path, size_t *length);

/**
 * Write data to a file at the given path.
 *
 * The file will be written to those platform `preferred write directory`:
 *
 * - **Linux:** `~/.local/share/<config->name>/`
 * - **Windows:** `C:\Users\<username>\AppData\Roaming\<config->name>\`
 * - **macOS:** `~/Library/Application Support/<config->name>/`
 *
 * `path` is the path to the file to write, relative to the preferred write
 * directory.
 *
 * `data` is a pointer to the data to write.
 *
 * `length` is the length of the data in bytes.
 *
 * `append` is a boolean flag indicating whether to append to the file (if true)
 * or overwrite it (if false).
 *
 * `return` true if the write operation was successful, false otherwise.  Use
 * `bxr_error_get` to get more information about the error.
 */
BXR_API bool
bxr_asset_write(const char *path, const void *data, size_t length, bool append);

#endif // BXR_ASSET_H_
