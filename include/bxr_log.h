/**
 * File `bxr_log.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_LOG_H_
#define BXR_LOG_H_

#include <SDL3/SDL.h>

#define BXR_LOG(level, ...)                                                    \
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, (level), __VA_ARGS__)
#define BXR_LOG_ERROR(...) BXR_LOG(SDL_LOG_PRIORITY_ERROR, __VA_ARGS__)
#define BXR_LOG_WARN(...) BXR_LOG(SDL_LOG_PRIORITY_WARN, __VA_ARGS__)
#define BXR_LOG_INFO(...) BXR_LOG(SDL_LOG_PRIORITY_INFO, __VA_ARGS__)
#define BXR_LOG_DEBUG(...) BXR_LOG(SDL_LOG_PRIORITY_DEBUG, __VA_ARGS__)

#endif // BXR_LOG_H_
