#include "dcimgui.h"

#include "engine.h"

#include "debug_gamepad.h"
#include "debug_keyboard.h"
#include "debug_mouse.h"
#include "debug_painter.h"

typedef struct debug_context_s debug_context_t;

struct debug_context_s
{
  bool show_debug;
  bool show_keyboard;
  bool show_mouse;
  bool show_gamepad;
  bool show_painter;
};

static debug_context_t _context = { 0 };

void
debug_entry_setup(void)
{
  _context.show_debug = true; // Show up the debug interface by default

  _context.show_keyboard = false;
  _context.show_mouse    = false;
  _context.show_gamepad  = false;
  _context.show_painter  = false;
}

void
debug_entry_frame(Uint64 ups, Uint64 fps)
{
  if (ImGui_IsKeyPressed(ImGuiKey_Escape)) {
    _context.show_debug = !_context.show_debug;
  }

  if (!_context.show_debug) {
    return;
  }

  ImGuiIO *io = ImGui_GetIO();

  ImGui_SetNextWindowPos((ImVec2){ 0, 0 }, ImGuiCond_Always);
  ImGui_SetNextWindowSize((ImVec2){ io->DisplaySize.x, io->DisplaySize.y },
                          ImGuiCond_Always);

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                           | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                           | ImGuiWindowFlags_NoScrollbar
                           | ImGuiWindowFlags_NoBringToFrontOnFocus
                           | ImGuiWindowFlags_NoBackground;

  flags |= ImGuiWindowFlags_MenuBar;

  ImGui_Begin("DevTools", NULL, flags);
  {
    if (ImGui_BeginMenuBar()) {
      if (ImGui_BeginMenu("Views")) {
        ImGui_MenuItemBoolPtr("Keyboard", NULL, &_context.show_keyboard, true);
        ImGui_MenuItemBoolPtr("Mouse", NULL, &_context.show_mouse, true);
        ImGui_MenuItemBoolPtr("Gamepad", NULL, &_context.show_gamepad, true);
        ImGui_MenuItemBoolPtr("Painter", NULL, &_context.show_painter, true);
        ImGui_EndMenu();
      }

      ImGui_EndMenuBar();
    }

    ImGui_Text("UPS: %lu", ups);
    ImGui_Text("FPS: %lu", fps);
  }

  if (_context.show_keyboard)
    debug_keyboard();

  if (_context.show_mouse)
    debug_mouse();

  if (_context.show_gamepad)
    debug_gamepad();

  if (_context.show_painter)
    debug_painter();

  ImGui_End();
}

void
debug_entry_shutdown(void)
{
}
