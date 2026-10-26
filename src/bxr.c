#include <SDL3/SDL.h>

#include <SDL3/SDL_init.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "bxr.h"

#include "bxr_sdl_gp.h"

// Log the last error as the reason of the startup failure.
static SDL_AppResult
init_failed_(void)
{
  BXR_LOG_ERROR("Initialization failed: %s", bxr_get_error());
  return SDL_APP_FAILURE;
}

static bool
update_refresh_rate_(bxr_context_t *context)
{
  SDL_DisplayID display_id = SDL_GetDisplayForWindow(context->window);
  if (display_id == 0) {
    BXR_LOG_WARN("Couldn't get display for window: %s", SDL_GetError());
    return false;
  }

  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display_id);
  if (mode == NULL) {
    BXR_LOG_WARN("Couldn't get display mode: %s", SDL_GetError());
    return false;
  }

  // SDL reports 0 when the refresh rate is unknown.
  if (mode->refresh_rate <= 0.0f) {
    BXR_LOG_WARN("Unknown display refresh rate");
    return false;
  }

  // Rounded to the nearest nanosecond and never 0, otherwise the fixed update
  // loop in SDL_AppIterate would never end.
  Uint64 delta_ns
      = (Uint64)SDL_lround((double)SDL_NS_PER_SECOND / mode->refresh_rate);
  context->target_delta_ns = SDL_max(delta_ns, 1);
  BXR_LOG_INFO("Updated display refresh rate: %.2f Hz (delta: %.2f ms)",
               mode->refresh_rate,
               (double)context->target_delta_ns / SDL_NS_PER_MS);

  return true;
}

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char **argv)
{
  const bxr_game_config_t *config = bxr_game_config();

  BXR_ASSERT(config);
  BXR_ASSERT(config->name);
  BXR_ASSERT(config->title);
  BXR_ASSERT(config->width > 0);
  BXR_ASSERT(config->height > 0);
  BXR_ASSERT(config->virtual_width > 0
             && config->virtual_width <= config->width);
  BXR_ASSERT(config->virtual_height > 0
             && config->virtual_height <= config->height);
  BXR_ASSERT(config->fullscreen == true || config->fullscreen == false);
  BXR_ASSERT(config->resizable == true || config->resizable == false);

  bxr_log_setup();

  bxr_context_t *context = bxr_context_get();
  if (!context) {
    return init_failed_();
  }

  context->config.name           = config->name;
  context->config.title          = config->title;
  context->config.width          = config->width;
  context->config.height         = config->height;
  context->config.virtual_width  = config->virtual_width;
  context->config.virtual_height = config->virtual_height;
  context->config.fullscreen     = config->fullscreen;
  context->config.resizable      = config->resizable;

// This is set because renderdoc doesn't support wayland
#if defined(__linux__) && defined(DEBUG)
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
#endif

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
    bxr_set_error("Couldn't initialize SDL (error: %s)", SDL_GetError());
    return init_failed_();
  }

  int flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;

  flags |= context->config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
  flags |= context->config.resizable ? SDL_WINDOW_RESIZABLE : 0;

  // Create a window
  context->window
      = SDL_CreateWindow(config->title, config->width, config->height, flags);
  if (context->window == NULL) {
    bxr_set_error("Failed to create window (error: %s)", SDL_GetError());
    return init_failed_();
  }

  // Create a GPU Device
  context->gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV
                                                | SDL_GPU_SHADERFORMAT_DXIL
                                                | SDL_GPU_SHADERFORMAT_MSL,
                                            true,
                                            NULL);
  if (context->gpu_device == NULL) {
    bxr_set_error("Failed to create GPU Device (error: %s)", SDL_GetError());
    return init_failed_();
  }

  BXR_LOG_INFO("Using GPU Driver: %s",
               SDL_GetGPUDeviceDriver(context->gpu_device));

  // Claime the window for the GPU device
  if (!SDL_ClaimWindowForGPUDevice(context->gpu_device, context->window)) {
    bxr_set_error("Failed to claim window for GPU device (error: %s)",
                  SDL_GetError());
    return init_failed_();
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

  BXR_LOG_INFO("Render pixel format: %s",
               SDL_GetPixelFormatName(context->pixel_format));

  // Default fixed timestep fallback (60 Hz)
  context->target_delta_ns = SDL_NS_PER_SECOND / 60;

  if (!update_refresh_rate_(context)) {
    BXR_LOG_WARN(
        "Failed to get display refresh rate, using default delta: %.2f ms",
        (double)context->target_delta_ns / SDL_NS_PER_MS);
  }

  // --- Init here ---

  bxr_painter_desc_t painter_desc = {
    .max_vertices = 0, // will use default
    .max_commands = 0, // will use default
    .window       = context->window,
    .gpu_device   = context->gpu_device,
  };

  bool is_ok = true;
  is_ok &= bxr_asset_setup(context);
  is_ok &= bxr_keyboard_setup();
  is_ok &= bxr_mouse_setup();
  is_ok &= bxr_gamepad_setup();
  is_ok &= bxr_shader_setup(context);
  is_ok &= bxr_audio_setup();
  is_ok &= bxr_image_setup();
  is_ok &= bxr_sampler_setup();
  is_ok &= bxr_painter_setup(&painter_desc);
  if (!is_ok) {
    return init_failed_();
  }

  // Call user setup
  bxr_game_setup();

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
  bxr_context_t *context = bxr_context_get();
  if (!context) {
    BXR_LOG_ERROR("Invalid context in SDL_AppIterate: %s", bxr_get_error());
    return SDL_APP_FAILURE;
  }

  static Uint64 fps            = 0;
  static Uint64 ups            = 0;
  static Uint64 fps_count      = 0;
  static Uint64 ups_count      = 0;
  static Uint64 lag_ns         = 0;
  static Uint64 last_ticks_ns  = 0;
  static Uint64 accumulator_ns = 0; // Track time for FPS counter
  static bool first_frame      = true;

  if (first_frame) {
    last_ticks_ns = SDL_GetTicksNS();
    first_frame   = false;
    return SDL_APP_CONTINUE;
  }

  Uint64 current_ticks_ns = SDL_GetTicksNS();
  Uint64 elapsed_ns       = current_ticks_ns - last_ticks_ns;

  last_ticks_ns = current_ticks_ns;

  lag_ns += elapsed_ns;
  accumulator_ns += elapsed_ns;

  Uint64 delta_ns = context->target_delta_ns;

  // Cap lag to avoid spiral of death (max  5 frames behind)
  Uint64 max_lag_ns = delta_ns * 5;
  if (lag_ns > max_lag_ns) {
    BXR_LOG_WARN("High lag detected: %.2f ms, capping to %.2f ms",
                 (double)lag_ns / SDL_NS_PER_MS,
                 (double)max_lag_ns / SDL_NS_PER_MS);
    lag_ns = max_lag_ns;
  }

  // Acquire a command buffer for the current frame
  SDL_GPUCommandBuffer *cmd_buffer
      = SDL_AcquireGPUCommandBuffer(context->gpu_device);
  if (!cmd_buffer) {
    bxr_set_error("Failed to acquire GPU command buffer (error: %s)",
                  SDL_GetError());
    BXR_LOG_ERROR("%s", bxr_get_error());
    return SDL_APP_FAILURE;
  }

  if (!SDL_GPBegin(context->config.virtual_width,
                   context->config.virtual_height)) {
    bxr_set_error("Failed to begin painter frame (error: %s)",
                  SDL_GPGetErrorMessage(SDL_GPGetLastError()));
    BXR_LOG_ERROR("%s", bxr_get_error());
    SDL_CancelGPUCommandBuffer(cmd_buffer);
    return SDL_APP_FAILURE;
  }
  {
    while (lag_ns >= delta_ns) {
      bxr_mouse_begin_frame();
      bxr_gamepad_begin_frame();
      bxr_audio_begin_frame();
      bxr_keyboard_begin_frame();

      // --- Update here (fixed timestep) ---

      bxr_game_update((double)delta_ns / SDL_NS_PER_MS); // this use SDL_GP API

      lag_ns -= delta_ns;
      ++ups_count;
    }

    double alpha = (double)lag_ns / (double)delta_ns;

    // --- Render here (use alpha for interpolation if needed) ---

    bxr_game_render(alpha); // this use SDL_GP API

    // Acquire the swapchain texture for the current frame
    SDL_GPUTexture *swapchain_texture = NULL;

    SDL_WaitAndAcquireGPUSwapchainTexture(
        cmd_buffer, context->window, &swapchain_texture, NULL, NULL);

    if (!swapchain_texture) {
      bxr_set_error("Failed to acquire swapchain texture (error: %s)",
                    SDL_GetError());
      BXR_LOG_ERROR("%s", bxr_get_error());
      SDL_CancelGPUCommandBuffer(cmd_buffer);
      return SDL_APP_FAILURE;
    }

    // Not fatal, the frame is dropped and the next one is tried
    if (!SDL_GPFlush(cmd_buffer, swapchain_texture)) {
      bxr_set_error("Failed to flush painter (error: %s)",
                    SDL_GPGetErrorMessage(SDL_GPGetLastError()));
      BXR_LOG_ERROR("%s", bxr_get_error());
    }
  }
  SDL_GPEnd();
  SDL_SubmitGPUCommandBuffer(cmd_buffer);

  ++fps_count;
  ++context->frame_count;

  // Calculate FPS and UPS every second
  if (accumulator_ns >= SDL_NS_PER_SECOND) {
    fps = fps_count;
    ups = ups_count;

    BXR_LOG_INFO("FPS: %" SDL_PRIu64 ", UPS: %" SDL_PRIu64, fps, ups);

    fps_count = 0;
    ups_count = 0;

    accumulator_ns -= SDL_NS_PER_SECOND;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
  BXR_ASSERT(event);

  bxr_context_t *context = bxr_context_get();

  bxr_gamepad_listen();

  switch (event->type) {
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    update_refresh_rate_(context);
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    context->config.width  = event->window.data1;
    context->config.height = event->window.data2;
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
                          event->button.clicks);
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    bxr_mouse_button_up((bxr_mouse_e)event->button.button);
    break;
  case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
    SDL_JoystickID id = event->gbutton.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index >= BXR_GAMEPAD_MAX)
      break;
    bxr_gamepad_button_down(index, event->gbutton.button);
  } break;
  case SDL_EVENT_GAMEPAD_BUTTON_UP: {
    SDL_JoystickID id = event->gbutton.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index >= BXR_GAMEPAD_MAX)
      break;
    bxr_gamepad_button_up(index, event->gbutton.button);
  } break;
  case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
    SDL_JoystickID id = event->gaxis.which;
    int index         = SDL_GetGamepadPlayerIndexForID(id);
    if (index < 0 || index >= BXR_GAMEPAD_MAX)
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

  bxr_game_teardown();

  bxr_audio_teardown();
  bxr_shader_teardown();
  bxr_sampler_teardown();
  bxr_image_teardown();
  bxr_gamepad_teardown();
  bxr_mouse_teardown();
  bxr_keyboard_teardown();
  bxr_asset_teardown();
  bxr_log_teardown();

  bxr_painter_teardown();

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

  BXR_FREE(context);
}

void
bxr_quit(void)
{
  SDL_Event quit_event = { 0 };
  quit_event.type      = SDL_EVENT_QUIT;
  if (!SDL_PushEvent(&quit_event)) {
    bxr_set_error("Failed to push quit event (error: %s)", SDL_GetError());
    BXR_LOG_ERROR("%s. Exiting immediately.", bxr_get_error());
    SDL_Quit();
  }
}
