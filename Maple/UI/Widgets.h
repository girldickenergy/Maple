#pragma once

#include <imgui.h>

class Widgets
{
public:
	static bool Tab(const char* label, void* icon, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg);
	static float CalcPanelHeight(int widgetCount, int textCount = 0, int spacingCount = 0);
	static void BeginPanel(const char* label, const ImVec2& size);
	static void EndPanel();
	static bool Checkbox(const char* label, bool* v);
};
