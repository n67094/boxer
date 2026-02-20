#include "dcimgui.h"

#include "debug_painter.h"

void
debug_painter()
{
  int flags = ImGuiWindowFlags_AlwaysAutoResize;
  ImGui_Begin("Painter", NULL, flags);
  {
  }
  ImGui_End();
}
