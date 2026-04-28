#include <SDL3/SDL.h>

#include <SDL3/SDL_init.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <physfs.h>

// Override SDL_gp configuration.

#define SDL_GP_PATH_MAX BXR_PATH_MAX_LENGTH
#define SDL_GP_TEXTURE_DIMENSION_MAX BXR_PAINTER_MAX_TEXTURE_DIMENSION
#define SDL_GP_IMAGE_MAX BXR_PAINTER_MAX_IMAGE
#define SDL_GP_SHADER_MAX BXR_PAINTER_MAX_SHADER
#define SDL_GP_PIPELINE_MAX BXR_PAINTER_MAX_PIPELINE
#define SDL_GP_STATE_MAX BXR_PAINTER_MAX_STATE
#define SDL_GP_TEXTURE_SLOTS_MAX BXR_PAINTER_MAX_TEXTURE_SLOTS
#define SDL_GP_TRANSFORMS_MAX BXR_PAINTER_MAX_TRANSFORMS
#define SDL_GP_UNIFORM_FLOATS_MAX BXR_PAINTER_MAX_UNIFORM_FLOATS

// Max number of commands that are looked back and batched together for
// optimization
#ifndef SDL_GP_OPTIMIZER_DEPTH
#define SDL_GP_OPTIMIZER_DEPTH 8
#endif

#include "bxr.h"

// Defined and included here so others SDL_gp.h includes inside bxr file don't
// re-include the implementation.
#define SDL_GP_IMPLEMENTATION
#include <SDL_gp.h>

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char **argv)
{
  bxr_game_config_t *config = bxr_game_config();

  SDL_assert(config);
  SDL_assert(config->name);
  SDL_assert(config->title);
  SDL_assert(config->width > 0);
  SDL_assert(config->height > 0);
  SDL_assert(config->fullscreen == true || config->fullscreen == false);
  SDL_assert(config->resizable == true || config->resizable == false);
  SDL_assert(config->target_ups > 0);

  if (!PHYSFS_init(NULL)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to init PhysFS (error: %s)",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return SDL_APP_FAILURE;
  }

  // PHYSFS get write directory (system specific)
  const char *write_dir = PHYSFS_getPrefDir("boxer_engine", config->name);
  if (!PHYSFS_setWriteDir(write_dir)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to set write dir to %s: (error: %s)",
                write_dir,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return SDL_APP_FAILURE;
  }

  SDL_LogInfo(
      SDL_LOG_CATEGORY_APPLICATION, "PhysFS write directory: %s", write_dir);

  // PHYSFS mounts write directory (system specific)
  if (!PHYSFS_mount(write_dir, NULL, 1)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to mount write dir %s: (error: %s)",
                write_dir,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return SDL_APP_FAILURE;
  }
  SDL_LogInfo(
      SDL_LOG_CATEGORY_APPLICATION, "PhysFS mount directory: %s", write_dir);

  // PHYSFS mounts read directory (BXR_DATA_DIR)
  const char *base_dir = PHYSFS_getBaseDir();
  char mount_dir[BXR_PATH_MAX_LENGTH];
  SDL_snprintf(mount_dir, BXR_PATH_MAX_LENGTH, "%s%s", base_dir, BXR_DATA_DIR);

  if (!PHYSFS_mount(mount_dir, "data/", 1)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to mount %s: (error: %s)",
                mount_dir,
                PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return SDL_APP_FAILURE;
  }
  SDL_LogInfo(
      SDL_LOG_CATEGORY_APPLICATION, "PhysFS mount directory: %s", mount_dir);

  // Try to read the config file (if exists) to override default configuration
  bxr_ini_reader_t *config_ini = bxr_ini_make_reader("config.ini");

  if (config_ini) {
    // Read width from ini
    int width = bxr_ini_read_number_or_else(
        config_ini, "window", "width", config->width);
    if (width != config->width) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Overriding config width from %d to %d",
                  config->width,
                  width);
      config->width = width;
    }

    // Read height from ini
    int height = bxr_ini_read_number_or_else(
        config_ini, "window", "height", config->height);
    if (height != config->height) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Overriding config height from %d to %d",
                  config->height,
                  height);
      config->height = height;
    }

    // Read fullscreen from ini
    bool fullscreen
        = bxr_ini_read_number_or_else(
              config_ini, "window", "fullscreen", config->fullscreen)
          != 0;
    if (fullscreen != config->fullscreen) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Overriding config fullscreen from %d to %d",
                  config->fullscreen,
                  fullscreen);
      config->fullscreen = fullscreen;
    }

    // Read resizable from ini
    bool resizable = bxr_ini_read_number_or_else(
                         config_ini, "window", "resizable", config->resizable)
                     != 0;
    if (resizable != config->resizable) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Overriding config resizable from %d to %d",
                  config->resizable,
                  resizable);
      config->resizable = resizable;
    }

    // Read target_ups from ini
    int target_ups = bxr_ini_read_number_or_else(
        config_ini, "window", "target_ups", config->target_ups);
    if (target_ups != config->target_ups) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Overriding config target_ups from %d to %d",
                  config->target_ups,
                  target_ups);
      config->target_ups = target_ups;
    }

    bxr_ini_destroy_reader(config_ini);
  }

  bxr_context_t *context = bxr_context_get();
  SDL_assert(context->_initialized == 0);

  context->_initialized = BXR_INIT_COOKIE;

  context->config.name  = config->name;
  context->config.title = config->title;

  context->config.width  = config->width;
  context->config.height = config->height;

  context->config.fullscreen = config->fullscreen;
  context->config.resizable  = config->resizable;

  context->config.target_ups = config->target_ups > 0 ? config->target_ups : 60;

  context->delta_ms = 1000 / context->config.target_ups;

// This is set because renderdoc doesn't support wayland
#if defined(__linux__) && defined(DEBUG)
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
#endif

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't initialize SDL: %s\n",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  int flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;

  flags |= context->config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
  flags |= context->config.resizable ? SDL_WINDOW_RESIZABLE : 0;

  // Create a window
  context->window
      = SDL_CreateWindow(config->title, config->width, config->height, flags);
  if (context->window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create window (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Create a GPU Device
  context->gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV
                                                | SDL_GPU_SHADERFORMAT_DXIL
                                                | SDL_GPU_SHADERFORMAT_MSL,
                                            true,
                                            NULL);
  if (context->gpu_device == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create GPU Device (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "Using GPU Driver: %s",
              SDL_GetGPUDeviceDriver(context->gpu_device));

  // Claime the window for the GPU device
  if (!SDL_ClaimWindowForGPUDevice(context->gpu_device, context->window)) {
    return SDL_APP_FAILURE;
  }

  // Set swapchain parameters
  SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_VSYNC;
  if (SDL_WindowSupportsGPUPresentMode(context->gpu_device,
                                       context->window,
                                       SDL_GPU_PRESENTMODE_IMMEDIATE)) {
    present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
  } else if (SDL_WindowSupportsGPUPresentMode(context->gpu_device,
                                              context->window,
                                              SDL_GPU_PRESENTMODE_MAILBOX)) {
    present_mode = SDL_GPU_PRESENTMODE_MAILBOX;
  }

  SDL_SetGPUSwapchainParameters(context->gpu_device,
                                context->window,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                present_mode);

  context->texture_format
      = SDL_GetGPUSwapchainTextureFormat(context->gpu_device, context->window);

  context->pixel_format
      = SDL_GetPixelFormatFromGPUTextureFormat(context->texture_format);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "Render pixel format: %s",
              SDL_GetPixelFormatName(context->pixel_format));

  SDL_GPUCommandBuffer *cmd_buffer
      = SDL_AcquireGPUCommandBuffer(context->gpu_device);
  if (cmd_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire GPU command buffer (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  context->cmd_buffer = cmd_buffer;
  bxr_painter_update_command_buffer(context->cmd_buffer);

  context->last_time_ms = SDL_GetTicks();

  bxr_painter_desc_t painter_desc = {
    .max_vertices = 0, // will use default
    .max_commands = 0, // will use default
    .window       = context->window,
    .gpu_device   = context->gpu_device,
  };

  bxr_painter_setup(&painter_desc);

  // Setup subsystems
  bxr_keyboard_setup();
  bxr_mouse_setup();
  bxr_gamepad_setup();
  bxr_image_setup(context);
  bxr_shader_setup(context);

  // Call user setup
  bxr_game_setup();

  // Submit for resources uploads during setup
  SDL_SubmitGPUCommandBuffer(context->cmd_buffer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
  SDL_assert(bxr_context_is_valid());

  bxr_context_t *context = bxr_context_get();

  static Uint64 fps_timer_ms = 0;

  static int update_count = 0;
  static int frame_count  = 0;

  float current_time_ms = SDL_GetTicks();
  float elapsed_time_ms = current_time_ms - context->last_time_ms;
  context->last_time_ms = current_time_ms;

  context->lag_ms += elapsed_time_ms;
  fps_timer_ms += elapsed_time_ms;

  // Cap lag to avoid spiral of death
  if (context->lag_ms > context->delta_ms * 3) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "High lag detected: %lu ms",
                context->lag_ms);
    context->lag_ms = context->delta_ms * 3;
  }

  // Update Logic (fixed timestep)

  while (context->lag_ms >= context->delta_ms) {
    bxr_keyboard_begin_frame();
    bxr_mouse_begin_frame();
    bxr_gamepad_begin_frame();

    bxr_game_update(context->delta_ms);

    context->lag_ms -= context->delta_ms;
    ++update_count;
  }

  // Render Game (variable timestep)

  // Get the command buffer
  context->cmd_buffer = SDL_AcquireGPUCommandBuffer(context->gpu_device);
  if (context->cmd_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire GPU command buffer (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  bxr_painter_update_command_buffer(context->cmd_buffer);

  // Get the tagert texture
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(context->cmd_buffer,
                                             context->window,
                                             &context->target_texture,
                                             NULL,
                                             NULL)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire swapchain texture (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  bxr_painter_update_swapchain_texture(context->target_texture);

  context->alpha_ms = context->lag_ms / context->delta_ms;

  bxr_game_render(context->alpha_ms);

  ++frame_count;

  SDL_SubmitGPUCommandBuffer(context->cmd_buffer);

  // Calculate FPS and UPS every second (1000 ms)
  if (fps_timer_ms >= 1000) {
    context->fps = frame_count;
    context->ups = update_count;

    frame_count  = 0;
    update_count = 0;

    fps_timer_ms -= 1000;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
  SDL_assert(event);
  SDL_assert(bxr_context_is_valid());

  bxr_gamepad_listen();

  switch (event->type) {
  case SDL_EVENT_WINDOW_RESIZED:
    bxr_context_set_window_dimensions(event->window.data1, event->window.data2);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    bxr_mouse_wheel_motion(event->wheel.x, event->wheel.y);
    break;
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    bxr_key_down((bxr_key_e)event->key.scancode);
    break;
  case SDL_EVENT_KEY_UP:
    bxr_key_up((bxr_key_e)event->key.scancode);
    break;
  case SDL_EVENT_MOUSE_MOTION:
    bxr_mouse_motion(event->motion.x, event->motion.y);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    bxr_mouse_button_down((bxr_mouse_e)event->button.button,
                          (bxr_mouse_click_e)event->button.clicks);
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    bxr_mouse_button_up((bxr_mouse_e)event->button.button);
    break;
  case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
    SDL_JoystickID id = event->gbutton.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index > BXR_GAMEPAD_MAX)
      break;
    bxr_gamepad_button_down(index, event->gbutton.button);
  } break;
  case SDL_EVENT_GAMEPAD_BUTTON_UP: {
    SDL_JoystickID id = event->gbutton.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index > BXR_GAMEPAD_MAX)
      break;
    bxr_gamepad_button_up(index, event->gbutton.button);
  } break;
  case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
    SDL_JoystickID id = event->gaxis.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index > BXR_GAMEPAD_MAX)
      break;
    bxr_gamepad_axis_motion(index, event->gaxis.axis, event->gaxis.value);
  } break;
  default:
    break;
  }

  return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  bxr_context_t *context = bxr_context_get();

  bxr_game_shutdown();

  bxr_keyboard_shutdown();
  bxr_mouse_shutdown();
  bxr_gamepad_shutdown();
  bxr_image_shutdown();
  bxr_shader_shutdown();

  if (context) {
    if (context->gpu_device && context->window) {
      SDL_ReleaseWindowFromGPUDevice(context->gpu_device, context->window);
    }

    if (context->gpu_device) {
      SDL_DestroyGPUDevice(context->gpu_device);
    }

    if (context->window) {
      SDL_DestroyWindow(context->window);
    }
  }

  PHYSFS_deinit();

  SDL_Quit();
}
