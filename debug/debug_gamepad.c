#include "dcimgui.h"

#include "engine.h"

#include "debug_gamepad.h"

static const char *_gamepad_button_names[ENGINE_GAMEPAD_BUTTON_SIZE] = {
  "South",          "East",          "West",           "North",
  "Back",           "Guide",         "Start",          "Left Stick",
  "Right Stick",    "Left Shoulder", "Right Shoulder", "DPad Up",
  "DPad Down",      "DPad Left",     "DPad Right",     "Misc1",
  "Right Paddle 1", "Left Paddle 1", "Right Paddle 2", "Left Paddle 2",
  "Touchpad",       "Misc2",         "Misc3",          "Misc4",
  "Misc5",          "Misc6",
};

static const char *_gamepad_axis_names[ENGINE_GAMEPAD_AXIS_SIZE] = {
  "Left X", "Left Y", "Right X", "Right Y", "Left Trigger", "Right Trigger",
};

void
debug_gamepad()
{
  int flags = ImGuiWindowFlags_AlwaysAutoResize;

  ImGui_Begin("Gamepad", NULL, flags);
  {
    int count = engine_gamepad_count();

    ImGui_Text("Gamepads: %d", count);
    ImGui_Text("Deadzone: %.2f", engine_gamepad_get_deadzone());

    if (count == 0) {
      ImGui_Text("No gamepads connected.");
      ImGui_End();
      return;
    }

    ImGui_BeginTable("##Gamepad-table",
                     ENGINE_GAMEPAD_MAX,
                     ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame
                         | ImGuiTableFlags_ScrollX);
    {
      for (int i = 0; i < ENGINE_GAMEPAD_MAX; ++i) {
        ImGui_TableNextColumn();

        if (!engine_gamepad_connected(i)) {
          ImVec2 avail_size = ImGui_GetContentRegionAvail();

          ImVec2 text_size = ImGui_CalcTextSize("Gamepad 0: Not Connected");

          float offset_x = ImGui_GetCursorPosX() + (avail_size.x * 0.5f)
                           - (text_size.x * 0.5f);
          float offset_y = ImGui_GetCursorPosY() + (avail_size.y * 0.5f)
                           - (text_size.y * 0.5f);

          ImGui_SetCursorPos((ImVec2){ offset_x, offset_y });
          ImGui_Text("Gamepad %d: Not Connected", i);
        } else {
          // Gamepad info
          if (engine_gamepad_connected(i)) {
            ImGui_Separator();
            ImGui_Text("Name: %s", engine_gamepad_name(i));

            ImGui_SeparatorText("Buttons");

            for (int j = 0; j < ENGINE_GAMEPAD_BUTTON_SIZE; ++j) {
              bool is_pressed = engine_gamepad_pressed(i, j);

              ImGui_Text("%s: ", _gamepad_button_names[j]);

              if (is_pressed) {
                ImGui_SameLine();
                ImGui_TextColored((ImVec4){ 0.0f, 1.0f, 0.0f, 1.0f },
                                  "Pressed");

                ImGui_SameLine();
                Uint64 held_time = engine_gamepad_held_time(i, j);
                ImGui_Text("(held for %lu ms)", held_time);
              } else {
                ImGui_SameLine();
                ImGui_TextColored((ImVec4){ 1.0f, 0.0f, 0.0f, 1.0f },
                                  "Released");
              }
            }

            ImGui_SeparatorText("Axes");
            for (int j = 0; j < ENGINE_GAMEPAD_AXIS_SIZE; ++j) {
              float value = engine_gamepad_axis(i, j);
              ImGui_Text("%s: %.2f", _gamepad_axis_names[j], value);
            }

            // Some calculations for button widths
            ImGuiStyle *style = ImGui_GetStyle();
            float avail_width = ImGui_GetContentRegionAvail().x;
            float button_width
                = (avail_width * 0.5f) - (style->ItemSpacing.x * 0.5f);

            static int rumble_low_frequency  = 50;
            static int rumble_high_frequency = 50;
            static int rumble_duration_ms    = 150;

            ImGui_SliderInt("Low Freq", &rumble_low_frequency, 0, 100);
            ImGui_SliderInt("High Freq", &rumble_high_frequency, 0, 100);
            ImGui_SliderInt("Duration (ms)", &rumble_duration_ms, 0, 5000);

            if (ImGui_ButtonEx("Stop", (ImVec2){ button_width, 0 })) {
              engine_gamepad_rumble(i, 0, 0, 0);
            }
            ImGui_SameLine();

            if (ImGui_ButtonEx("Rumble", (ImVec2){ button_width, 0 })) {
              engine_gamepad_rumble(i,
                                    rumble_low_frequency,
                                    rumble_high_frequency,
                                    rumble_duration_ms);
            }
          }
        }
      }
    }
    ImGui_EndTable();
  }
  ImGui_End();
}
