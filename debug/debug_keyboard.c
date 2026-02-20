#include "dcimgui.h"

#include "engine.h"

#include "debug_keyboard.h"

void
debug_keyboard()
{
  int flags = ImGuiWindowFlags_AlwaysAutoResize;
  ImGui_Begin("Keyboard", NULL, flags);
  {
    for (int i = 0; i < ENGINE_KEY_SIZE; ++i) {
      const char *name = engine_key_name((engine_key_e)i);
      ImGui_Text("%s", name);

      bool is_pressed = engine_key_pressed((engine_key_e)i);
      if (is_pressed) {
        ImGui_SameLine();
        ImGui_TextColored((ImVec4){ 0.0f, 1.0f, 0.0f, 1.0f }, "Pressed");

        ImGui_SameLine();
        Uint64 held_time = engine_key_held_time((engine_key_e)i);
        ImGui_Text("(held for %lu ms)", held_time);
      } else {
        ImGui_SameLine();
        ImGui_TextColored((ImVec4){ 1.0f, 0.0f, 0.0f, 1.0f }, "Released");
      }
    }
  }
  ImGui_End();
}
