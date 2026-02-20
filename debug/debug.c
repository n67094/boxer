#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <dcimgui.h>
#include <dcimgui_impl_sdl3.h>
#include <dcimgui_impl_sdlgpu3.h>

#include "engine_config.h"
#include "engine_context.h"

#include "debug.h"
#include "debug_entry.h"
#include "debug_font.h"
#include "debug_theme.h"

ImGuiContext *g_imgui_context = NULL;

static Uint32 _initialized = 0;

void
debug_setup(engine_context_t *context)
{
  SDL_assert(context);

  g_imgui_context = ImGui_CreateContext(NULL);
  ImGuiIO *io     = ImGui_GetIO();

  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  cImGui_ImplSDL3_InitForSDLGPU(context->window);

  ImGui_ImplSDLGPU3_InitInfo info = {
    .Device            = context->device,
    .ColorTargetFormat = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
    .MSAASamples       = SDL_GPU_SAMPLECOUNT_1,
  };

  cImGui_ImplSDLGPU3_Init(&info);

  ImGui_StyleColorsDark(NULL);

  debug_font_setup();
  debug_theme_setup();
  debug_entry_setup();

  _initialized = ENGINE_INIT_COOKIE;
}

void
debug_event(SDL_Event *event)
{
  cImGui_ImplSDL3_ProcessEvent(event);
}

void
debug_render(engine_context_t *context)
{
  SDL_assert(context);
  SDL_assert(_initialized == ENGINE_INIT_COOKIE);

  cImGui_ImplSDLGPU3_NewFrame();
  cImGui_ImplSDL3_NewFrame();

  ImGui_NewFrame();

  debug_entry_frame(context->ups, context->fps);

  ImGui_Render();

  ImDrawData *draw_data = ImGui_GetDrawData();

  cImGui_ImplSDLGPU3_PrepareDrawData(draw_data, context->cmd_buffer);

  SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(
      context->cmd_buffer,
      &(SDL_GPUColorTargetInfo){ .texture     = context->target_texture,
                                 .cycle       = false,
                                 .load_op     = SDL_GPU_LOADOP_LOAD,
                                 .store_op    = SDL_GPU_STOREOP_STORE,
                                 .clear_color = { 0, 0, 0, 1 } },
      1,
      NULL);

  cImGui_ImplSDLGPU3_RenderDrawData(
      draw_data, context->cmd_buffer, render_pass);

  SDL_EndGPURenderPass(render_pass);
}

void
debug_shutdown()
{
  debug_entry_shutdown();

  cImGui_ImplSDL3_Shutdown();
  cImGui_ImplSDLGPU3_Shutdown();

  ImGui_DestroyContext(g_imgui_context);

  _initialized = 0;
}
