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
#define SDL_GP_OPTIMIZER_DEPTH BXR_PAINTER_MAX_OPTIMIZER_DEPTH

#include "bxr.h"

// SDL_gp is defined and included here to prevent duplicate implementation
// inclusions from other SDL_gp headers within the bxr file.
#define SDL_GP_IMPLEMENTATION
#include <SDL_gp.h>

static bool
update_refresh_rate_(bxr_context_t *context)
{
  SDL_DisplayID display_id = SDL_GetDisplayForWindow(context->window);
  if (display_id == 0) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't get display for window: %s",
                SDL_GetError());
    return false;
  }

  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display_id);
  if (mode == NULL) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't get display mode: %s",
                SDL_GetError());
    return false;
  }

  // context->target_delta_ms = 1000.0 / (double)mode->refresh_rate;
  context->target_delta_ms = 1000.0 / (double)mode->refresh_rate;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "Updated display refresh rate: %.2f Hz (delta: %2.f ms)",
              mode->refresh_rate,
              context->target_delta_ms);

  return true;
}

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

  bxr_context_t *context = bxr_context_get();

  context->config.name       = config->name;
  context->config.title      = config->title;
  context->config.width      = config->width;
  context->config.height     = config->height;
  context->config.fullscreen = config->fullscreen;
  context->config.resizable  = config->resizable;

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

  // Default fixed timestep fallback (60 Hz)
  context->target_delta_ms = 1000.0 / 60.0;

  if (!update_refresh_rate_(context)) {
    SDL_LogWarn(
        SDL_LOG_CATEGORY_APPLICATION,
        "Failed to get display refresh rate, using default delta: %2.f ms",
        context->target_delta_ms);
  }

  // --- Init here ---

  SDL_GPUCommandBuffer *cmd_buffer
      = SDL_AcquireGPUCommandBuffer(context->gpu_device);
  if (cmd_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire GPU command buffer (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  bxr_painter_update_command_buffer(cmd_buffer);

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
  SDL_SubmitGPUCommandBuffer(cmd_buffer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
  bxr_context_t *context = bxr_context_get();
  if (!context) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Invalid context in SDL_AppIterate");
    return SDL_APP_FAILURE;
  }

  static Uint64 fps            = 0;
  static Uint64 ups            = 0;
  static Uint64 frame_count    = 0;
  static Uint64 update_count   = 0;
  static double lag_ms         = 0;
  static double last_ticks     = 0;
  static Uint64 accumulator_ms = 0; // Track time for FPS counter
  static bool first_frame      = true;

  if (first_frame) {
    last_ticks  = SDL_GetTicks();
    first_frame = false;
    return SDL_APP_CONTINUE;
  }

  Uint64 current_ticks = SDL_GetTicks();
  double elapsed_ms    = (double)(current_ticks - last_ticks);

  last_ticks = current_ticks;

  lag_ms += elapsed_ms;
  accumulator_ms += elapsed_ms;

  double delta_ms = context->target_delta_ms;

  // Cap lag to avoid spiral of death (max  5 frames behind)
  double max_lag_ms = delta_ms * 5.0;
  if (lag_ms > delta_ms * 5.0) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "High lag detected: %.2f ms, capping to %.2f ms",
                lag_ms,
                max_lag_ms);
    lag_ms = max_lag_ms;
  }

  while (lag_ms >= delta_ms) {
    // --- Update here (fixed timestep) ---

    bxr_keyboard_begin_frame();
    bxr_mouse_begin_frame();
    bxr_gamepad_begin_frame();

    bxr_game_update(delta_ms);

    lag_ms -= delta_ms;
    ++update_count;
  }

  double alpha_ms = (double)(lag_ms / delta_ms);

  // --- Render here (use alpha_ms for interpolation if needed) ---

  // Get the command buffer
  SDL_GPUCommandBuffer *cmd_buffer
      = SDL_AcquireGPUCommandBuffer(context->gpu_device);
  if (cmd_buffer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire GPU command buffer (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  bxr_painter_update_command_buffer(cmd_buffer);

  // Get the tagert texture
  SDL_GPUTexture *target_texture = NULL;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(
          cmd_buffer, context->window, &target_texture, NULL, NULL)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to acquire swapchain texture (error: %s)",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }
  bxr_painter_update_swapchain_texture(target_texture);

  bxr_game_render(alpha_ms);

  SDL_SubmitGPUCommandBuffer(cmd_buffer);

  ++frame_count;

  // Calculate FPS and UPS every second
  if (accumulator_ms >= 1000) {
    fps = frame_count;
    ups = update_count;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FPS: %lu, UPS: %lu", fps, ups);

    frame_count  = 0;
    update_count = 0;
    accumulator_ms -= 1000;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
  SDL_assert(event);

  bxr_context_t *context = bxr_context_get();

  bxr_gamepad_listen();

  switch (event->type) {
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Display changed, updating refresh rate...");
    update_refresh_rate_(context);
    break;
  }
  case SDL_EVENT_WINDOW_RESIZED:
    bxr_context_set_window_dimensions(event->window.data1, event->window.data2);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    bxr_mouse_wheel_motion(event->wheel.x, event->wheel.y);
    break;
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
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
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

  bxr_painter_shutdown();

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
}

void
bxr_quit(void)
{
  SDL_Event quit_event;
  quit_event.type = SDL_EVENT_QUIT;
  SDL_PushEvent(&quit_event);
}
