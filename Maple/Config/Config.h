#pragma once

#include "imgui.h"

class Config
{
public:
	struct Timewarp
	{
		static inline bool Enabled = false;
		static inline int Rate = 100;
	};

	struct Visuals
	{
		static inline bool ARChangerEnabled = false;
		static inline bool ARChangerAdjustToMods = false;
		static inline float AR = 9.2f;
		static inline bool HiddenDisabled = false;
		static inline bool FlashlightDisabled = false;
		static inline int MenuScale = 2;
		static inline ImVec4 AccentColour = ImColor(232, 93, 155, 255).Value;
		static inline ImVec4 MenuColour = ImColor(65, 65, 65, 255).Value;
		static inline ImVec4 ControlColour = ImColor(76, 76, 76, 255).Value;
		static inline ImVec4 TextColour = ImColor(255, 255, 255, 255).Value;
	};

	struct Misc
	{
		static inline bool DisableSpectators = false;
	};
};
