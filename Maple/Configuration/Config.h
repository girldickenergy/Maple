#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "EncryptedString.h"
#include "imgui.h"
#include "xorstr.hpp"

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
			EncryptedString MenuBackground;
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
			EncryptedString CustomLargeImageText = xorstr_("peppy (rank #18,267,309)");
			bool CustomPlayModeEnabled = false;
			int CustomPlayMode = 0;
			bool CustomStateEnabled = false;
			EncryptedString CustomState = xorstr_("Trying to detect Maple since 2021");
			bool CustomDetailsEnabled = false;
			EncryptedString CustomDetails = xorstr_("Kenji Ninuma - DISCO PRINCE [Normal]");
			bool HideSpectateButton = false;
			bool HideMatchButton = false;
		} DiscordRichPresenceSpoofer;
	} Misc;

	void Serialize(std::ostream& outStream)
	{
        outStream.write(reinterpret_cast<const char*>(&Version), sizeof(float));
		outStream.write(reinterpret_cast<const char*>(&Relax.Enabled), reinterpret_cast<uintptr_t>(&Visuals.UI.MenuBackground) - reinterpret_cast<uintptr_t>(&Relax.Enabled));
		Visuals.UI.MenuBackground.Serialize(outStream);
		outStream.write(reinterpret_cast<const char*>(&Visuals.UI.Snow), reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomLargeImageText) - reinterpret_cast<uintptr_t>(&Visuals.UI.Snow));
		Misc.DiscordRichPresenceSpoofer.CustomLargeImageText.Serialize(outStream);
		outStream.write(reinterpret_cast<const char*>(&Misc.DiscordRichPresenceSpoofer.CustomPlayModeEnabled), reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomState) - reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomPlayModeEnabled));
		Misc.DiscordRichPresenceSpoofer.CustomState.Serialize(outStream);
		outStream.write(reinterpret_cast<const char*>(&Misc.DiscordRichPresenceSpoofer.CustomDetailsEnabled), sizeof(bool));
		Misc.DiscordRichPresenceSpoofer.CustomDetails.Serialize(outStream);
		outStream.write(reinterpret_cast<const char*>(&Misc.DiscordRichPresenceSpoofer.HideSpectateButton), sizeof(bool) * 2);
	}

	void Deserialize(std::istream& inStream)
	{
        float version;
		inStream.read(reinterpret_cast<char*>(&version), sizeof(float));
        if (version == 2.f)
        {
            Version = version;
			inStream.read(reinterpret_cast<char*>(&Relax.Enabled), reinterpret_cast<uintptr_t>(&Visuals.UI.MenuBackground) - reinterpret_cast<uintptr_t>(&Relax.Enabled));
			Visuals.UI.MenuBackground.Deserialize(inStream);
			inStream.read(reinterpret_cast<char*>(&Visuals.UI.Snow), reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomLargeImageText) - reinterpret_cast<uintptr_t>(&Visuals.UI.Snow));
			Misc.DiscordRichPresenceSpoofer.CustomLargeImageText.Deserialize(inStream);
			inStream.read(reinterpret_cast<char*>(&Misc.DiscordRichPresenceSpoofer.CustomPlayModeEnabled), reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomState) - reinterpret_cast<uintptr_t>(&Misc.DiscordRichPresenceSpoofer.CustomPlayModeEnabled));
			Misc.DiscordRichPresenceSpoofer.CustomState.Deserialize(inStream);
			inStream.read(reinterpret_cast<char*>(&Misc.DiscordRichPresenceSpoofer.CustomDetailsEnabled), sizeof(bool));
			Misc.DiscordRichPresenceSpoofer.CustomDetails.Deserialize(inStream);
			inStream.read(reinterpret_cast<char*>(&Misc.DiscordRichPresenceSpoofer.HideSpectateButton), sizeof(bool) * 2);
		}
	}
};