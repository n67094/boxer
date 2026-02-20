#include "dcimgui.h"

#include "engine.h"

#include "debug_font.h"
#include "debug_icons.h"

static const char *TEXT_FONT = "debug/fonts/ProggyCleanCENerdFont-Regular.ttf";
static const char *ICON_FONT = "debug/fonts/fa-solid-900.ttf";

static ImFont *font             = { 0 };
static ImFontConfig font_config = { 0 };

static ImFont *icon_font             = { 0 };
static ImFontConfig icon_font_config = { 0 };

void
debug_font_setup(void)
{
  ImGuiIO *io = ImGui_GetIO();

  char icon_font_path[256];
  char text_font_path[256];

  const char *base_path = SDL_GetBasePath();

  SDL_snprintf(
      text_font_path, sizeof(text_font_path), "%s%s", base_path, TEXT_FONT);
  SDL_snprintf(
      icon_font_path, sizeof(icon_font_path), "%s%s", base_path, ICON_FONT);

  // Clear the font atlas
  // ImFontAtlas_Clear(io->Fonts);

  // Configure main font
  font_config.FontDataOwnedByAtlas
      = true; // Font data is owned by the atlas no need to free it
  // font_config.OversampleH      = 0;
  // font_config.OversampleV      = 0;
  // font_config.PixelSnapH         = true;
  font_config.GlyphMaxAdvanceX   = 13.0f;
  font_config.RasterizerDensity  = 1.0f;
  font_config.RasterizerMultiply = 1.0f;

  font = ImFontAtlas_AddFontFromFileTTF(
      io->Fonts, text_font_path, 13.0f, &font_config, NULL);

  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to add font!");
    return;
  }

  // Configure icon font - with more conservative settings
  icon_font_config.MergeMode = true; // Merge with the previous font
  icon_font_config.FontDataOwnedByAtlas
      = true; // Font data is owned by the atlas no need to free it
  icon_font_config.OversampleH        = 1;
  icon_font_config.OversampleV        = 1;
  icon_font_config.PixelSnapH         = true;
  icon_font_config.GlyphMaxAdvanceX   = 16.0f;
  icon_font_config.GlyphOffset.y      = 2.0f;  // Offset vertically by 1px
  icon_font_config.GlyphOffset.x      = -2.0f; // Offset horizontally by -2px
  icon_font_config.RasterizerDensity  = 4.0f;
  icon_font_config.RasterizerMultiply = 1.0f;

  ImWchar icon_ranges[] = { DEBUG_ICON_MIN_FA, DEBUG_ICON_MAX_16_FA, 0 };
  icon_font             = ImFontAtlas_AddFontFromFileTTF(
      io->Fonts,
      icon_font_path,
      16.0f, // Make icon font of thesame size as font
      &icon_font_config,
      icon_ranges);

  if (!icon_font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to add icon font!");
    return;
  }

  // ImFontAtlas_ClearTexData(io->Fonts);
  ImFontAtlas_Build(io->Fonts);
}
