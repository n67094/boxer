#include "dcimgui.h"

#include "engine.h"

#include "debug_mouse.h"

static const char *_mouse_button_names[ENGINE_MOUSE_SIZE] = {
  "Unknown",      "Left Button", "Middle Button",
  "Right Button", "X1 Button",   "X2 Button",
};

void
debug_mouse()
{
  int flags = ImGuiWindowFlags_AlwaysAutoResize;
  ImGui_Begin("Mouse", NULL, flags);
  {
    ImGui_Text("Position: (%.2f, %.2f)", engine_mouse_x(), engine_mouse_y());
    ImGui_Text("Wheel Motion: (%.2f, %.2f)",
               engine_mouse_wheel_motion_x(),
               engine_mouse_wheel_motion_y());

    for (int i = 0; i < ENGINE_MOUSE_SIZE; ++i) {
      if (i == ENGINE_MOUSE_UNKNOWN) {
        continue;
      }

      const char *button_name = _mouse_button_names[i];

      ImGui_Text("%s", button_name);

      if (engine_mouse_double_clicked((engine_mouse_e)i)) {
        ImGui_SameLine();
        ImGui_TextColored((ImVec4){ 0.0f, 1.0f, 0.0f, 1.0f }, "Double Clicked");

        ImGui_SameLine();
        Uint64 held_time = engine_mouse_held_time((engine_mouse_e)i);
        ImGui_Text("(held for %lu ms)", held_time);
      } else if (engine_mouse_pressed((engine_mouse_e)i)) {
        ImGui_SameLine();
        ImGui_TextColored((ImVec4){ 0.0f, 1.0f, 0.0f, 1.0f }, "Pressed");

        ImGui_SameLine();
        Uint64 held_time = engine_mouse_held_time((engine_mouse_e)i);
        ImGui_Text("(held for %lu ms)", held_time);
      } else {
        ImGui_SameLine();
        ImGui_TextColored((ImVec4){ 1.0f, 0.0f, 0.0f, 1.0f }, "Released");
      }
    }
  }
  ImGui_End();
}
