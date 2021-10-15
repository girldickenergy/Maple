#pragma once

#include <imgui.h>

class Widgets
{
public:
	static bool Tab(const char* label, void* icon, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg);
};
