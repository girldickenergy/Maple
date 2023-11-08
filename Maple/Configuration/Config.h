#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "imgui.h"

struct Config
{
	float Version = 2.f;

	struct Relax
	{
		bool Enabled = false;
		int ToggleKey = VK_RCONTROL;
		int PrimaryKey = 1;
		int SecondaryKey = 3;
		int AlternateBPM = 100;
		bool SliderAlternationOverride = false;

		struct Timing
		{
			int Offset = 0;
			int TargetUnstableRate = 100;
			int AllowableHitRange = 300;
			int MinimumHoldTime = 60;
			int MaximumHoldTime = 80;
			int MinimumSliderHoldTime = 50;
			int MaximumSliderHoldTime = 65;
		} Timing;

		struct HitScan
		{
			bool DirectionPredictionEnabled = true;
			int DirectionPredictionAngle = 45;
			float DirectionPredictionScale = 0.7f;
		} HitScan;

		struct Blatant
		{
			bool UseLowestPossibleHoldTimes = false;
		} Blatant;
	} Relax;

	struct AimAssist
	{
		bool Enabled = false;
		int Algorithm = 0;

		struct Algorithmv1
		{
			float Strength = 0.6f;
			bool AssistOnSliders = true;
			int BaseFOV = 45;
			float MaximumFOVScale = 2.5f;
			float MinimumFOVTotal = 0.f;
			float MaximumFOVTotal = 225.f;
			float AccelerationFactor = 1.f;
		} Algorithmv1;

		struct Algorithmv2
		{
			float Power = 0.5f;
			bool AssistOnSliders = true;
		} Algorithmv2;

		struct Algorithmv3
		{
			float Power = 0.5f;
			float SliderAssistPower = 1.f;
		} Algorithmv3;

		bool DrawDebugOverlay = false;
	} AimAssist;

	struct Timewarp
	{
		bool Enabled = false;
		int Type = 0;
		int Rate = 100;
		float Multiplier = 1.;
		bool RateLimitEnabled = true;
	} Timewarp;

	struct Visuals
	{
		struct ARChanger
		{
			bool Enabled = false;
			float AR = 9.2f;
			bool AdjustToMods = false;
			bool AdjustToRate = false;
			bool DrawPreemptiveDot = false;
			ImVec4 PreemptiveDotColour = ImColor(232, 93, 155, 255).Value;
		} ARChanger;

		struct Removers
		{
			bool HiddenRemoverEnabled = false;
			bool FlashlightRemoverEnabled = false;
		} Removers;

		struct CSChanger
		{
			bool Enabled = false;
			float CS = 4.2;
		} CSChanger;

		struct UI
		{
			int MenuScale = 2;
			char MenuBackground[MAX_PATH] = "\0";
			bool Snow = false;
			ImVec4 AccentColour = ImColor(232, 93, 155, 255).Value;
			ImVec4 MenuColour = ImColor(65, 65, 65, 255).Value;
			ImVec4 ControlColour = ImColor(76, 76, 76, 255).Value;
			ImVec4 TextColour = ImVec4(ImColor(255, 255, 255, 255));
		} UI;
	} Visuals;

	struct Misc
	{
		int ScoreSubmissionType = 0;
		int PromptBehaviorOnRetry = 0;
		bool DisableSpectators = false;

		struct DiscordRichPresenceSpoofer
		{
			bool Enabled = false;
			bool CustomLargeImageTextEnabled = false;
			char CustomLargeImageText[128] = "peppy (rank #18,267,309)";
			bool CustomPlayModeEnabled = false;
			int CustomPlayMode = 0;
			bool CustomStateEnabled;
			char CustomState[128] = "Trying to detect Maple since 2021";
			bool CustomDetailsEnabled = false;
			char CustomDetails[128] = "Kenji Ninuma - DISCO PRINCE [Normal]";
			bool HideSpectateButton = false;
			bool HideMatchButton = false;
		} DiscordRichPresenceSpoofer;
	} Misc;
};