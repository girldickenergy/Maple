#pragma once

#include <string>

#include "imgui.h"

class Config
{
public:
	struct AimAssist
	{
		static inline bool Enabled = false;
		static inline bool EasyMode = true;
		/* Easy mode arguments*/
		static inline float EasyModeStrength = 0.5f;
		/* Advanced mode arguments*/
		static inline float Strength = 0.6f;
		static inline int BaseFOV = 45;
		static inline float MaximumFOVScale = 2.5f;
		static inline float MinimumFOVTotal = 0.f;
		static inline float MaximumFOVTotal = 225.f;
		static inline float SliderballDeadzone = 12.5f;
		static inline bool FlipSliderballDeadzone = true;
		static inline float StrengthMultiplier = 1.f;
		static inline float AssistDeadzone = 3.f;
		static inline float ResyncLeniency = 3.5f;
		static inline float ResyncLeniencyFactor = 0.698f;
		static inline bool AssistOnSliders = true;
		static inline bool DrawDebugOverlay = false;
	};

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
		static inline bool PromptOnScoreSubmissionEnabled = false;
		static inline bool RichPresenceSpooferEnabled = false;
		static inline char SpoofedName[64] = "maple.software";
		static inline char SpoofedRank[64] = "rocking osu! since 2021";
	};
};
