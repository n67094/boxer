/**
 * File `bxr_asset.h`.
 *
 * # Game Asset Management.
 *
 * Built on PhysicsFS. Directories and archives are mounted into a single
 * read-only search path, and one write directory.
 *
 * Lookup order when a path exists in several places:
 *
 * 1. The write directory (mounted first by `bxr_asset_setup()`).
 * 2. Mounted directories and archives, in the order they were mounted.
 *
 * Supported archive types by PhysicsFS:
 *
 * - .ZIP (pkZip/WinZip/Info-ZIP compatible)
 * - .7Z (7zip archives)
 * - .ISO (ISO9660 files, CD-ROM images)
 * - .GRP (Build Engine groupfile archives)
 * - .PAK (Quake I/II archive format)
 * - .HOG (Descent I/II/III HOG file archives)
 * - .MVL (Descent II movielib archives)
 * - .WAD (DOOM engine archives)
 * - .BIN (Chasm: The Rift engine archives)
 * - .VDF (Gothic I/II engine archives)
 * - .SLB (Independence War archives)
 * - .POD (Terminal Velocity POD archives)
 * - .DAT (Resident Evil 3 rofs.dat archives)
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ASSET_H_
#define BXR_ASSET_H_

#include <SDL3/SDL.h>

#include "bxr_context.h"
#include "bxr_defs.h"

bool bxr_asset_setup(bxr_context_t *context);
void bxr_asset_teardown(void);

/**
 * ## Public API
 */

/**
 * Add a directory or archive to the end of the read search path.
 *
 * Its content is mounted at the root of the virtual file system and merged with
 * what is already mounted. When a file exists in several places, the write
 * directory wins, then the earliest mounted entry.
 *
 * Mounting an already mounted path does nothing and succeeds.
 *
 * `path` directory or archive to mount, in platform-dependent notation. Must
 * not be NULL.
 *
 * `return` true on success, false on failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool bxr_asset_mount(const char *path);

/**
 * Remove a directory or archive from the read search path.
 *
 * `path` directory or archive to unmount, exactly as given to
 * `bxr_asset_mount()`. Must not be NULL.
 *
 * `return` true on success, false if it was not mounted or is still in use.
 */
BXR_API bool bxr_asset_unmount(const char *path);

/**
 * Check if a file or directory exists in the virtual file system.
 *
 * Searches the write directory and every mounted directory and archive.
 *
 * `path` virtual path to check. Must not be NULL.
 *
 * `return` true if it exists, false otherwise.
 */
BXR_API bool bxr_asset_exists(const char *path);

/**
 * Check if a path ends with the given extension.
 *
 * Pure string check, the file does not need to exist and the asset subsystem
 * does not need to be initialized.
 *
 * Comparison is case-insensitive and only looks at the last extension of the
 * file name (`a.tar.gz` has `.gz`). A file name starting with a dot and no
 * other dot (`.hidden`) has no extension.
 *
 * `path` path to check. Must not be NULL.
 * `extension` extension including the leading dot (e.g. ".png"). Must not be
 * NULL.
 *
 * `return` true if the extension matches, false otherwise.
 */
BXR_API bool bxr_asset_has_extension(const char *path, const char *extension);

/**
 * Check if a path in the virtual file system is a directory.
 *
 * `path` virtual path to check. Must not be NULL.
 *
 * `return` true if it is a directory, false if it is not or does not exist.
 */
BXR_API bool bxr_asset_is_directory(const char *path);

/**
 * Read a whole file from the virtual file system into memory.
 *
 * The returned buffer is not NUL-terminated; do not use it as a C string
 * without copying it.
 *
 * `path` virtual path of the file to read. Must not be NULL.
 * `length` receives the size of the returned data in bytes. May be NULL. Left
 * untouched on failure.
 *
 * `return` the file contents, or NULL if the file does not exist, is empty, or
 * cannot be read.
 * Call `bxr_get_error()` for more information.
 *
 * **Note**: the caller must free the result with `BXR_FREE()`.
 */
BXR_API Uint8 *bxr_asset_read(const char *path, size_t *length);

/**
 * Write data to a file in the write directory.
 *
 * The write directory is the platform user data directory:
 *
 * - **Linux:** `~/.local/share/<config.name>/`
 * - **Windows:** `C:\Users\<username>\AppData\Roaming\boxer\<config.name>\`
 * - **macOS:** `~/Library/Application Support/<config.name>/`
 *
 * The file is created if needed, but parent directories must already exist.
 *
 * Since the write directory has the highest lookup priority, the written file
 * hides any mounted file with the same path.
 *
 * `path` virtual path of the file, relative to the write directory. Must not be
 * NULL.
 * `data` bytes to write. Must not be NULL.
 * `length` number of bytes to write. Must be greater than 0.
 * `append` true to append to the end of the file, false to truncate it first.
 *
 * `return` true on success, false on failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool
bxr_asset_write(const char *path, const void *data, size_t length, bool append);

/**
 * Delete a file or an empty directory from the write directory.
 *
 * See `bxr_asset_write()` for the write directory location. Mounted
 * directories and archives are never modified.
 *
 * `path` virtual path to delete, relative to the write directory. Must not be
 * NULL.
 *
 * `return` true on success, false on failure.
 * Call `bxr_get_error()` for more information.
 */
BXR_API bool bxr_asset_delete(const char *path);

#endif // BXR_ASSET_H_
