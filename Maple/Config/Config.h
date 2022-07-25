#pragma once

#include <comdef.h>
#include <string>
#include <vector>

#include "imgui.h"

class Config
{
	static ImVec4 parseImVec4(std::string vec);
	static void loadDefaults();
public:
	static inline std::vector<std::string> Configs;
	static inline int CurrentConfig = 0;
	static inline char NewConfigName[24] = { };

	static void Initialize();
	static void Load();
	static void Save();
	static void Create();
	static void Refresh();

	//Version 0.0 - first version
	//Version 1.0 - breaks aim assist config compatibility (removed aav1, added aav3 (aqn))
	static inline constexpr float VERSION = 1.f;

	struct Relax
	{
		static inline bool Enabled;
		static inline int ToggleKey;
		static inline int PrimaryKey;
		static inline int SecondaryKey;
		static inline int AlternateBPM;
		static inline bool SliderAlternationOverride;

		struct Timing
		{
			static inline int Offset;
			static inline int TargetUnstableRate;
			static inline int AverageHoldTime;
			static inline int AverageHoldTimeError;
			static inline int AverageSliderHoldTime;
			static inline int AverageSliderHoldTimeError;
		};

		struct HitScan
		{
			static inline bool WaitLateEnabled;
			static inline bool DirectionPredictionEnabled;
			static inline int DirectionPredictionAngle;
			static inline float DirectionPredictionScale;
		};
	};

	struct AimAssist
	{
		static inline bool Enabled;
		static inline int Algorithm;

		struct Algorithmv1
		{
			static inline float Strength;
			static inline bool AssistOnSliders;
			static inline int BaseFOV;
			static inline float MaximumFOVScale;
			static inline float MinimumFOVTotal;
			static inline float MaximumFOVTotal;
			static inline float AccelerationFactor;
		};

		struct Algorithmv2
		{
			static inline float Power;
			static inline bool AssistOnSliders;
		};

		struct Algorithmv3
		{
			static inline float Power;
			static inline float SliderAssistPower;
		};

		static inline bool DrawDebugOverlay;
	};

	struct Timewarp
	{
		static inline bool Enabled;
		static inline int Type;
		static inline int Rate;
		static inline float Multiplier;
	};

	struct Visuals
	{
		struct ARChanger
		{
			static inline bool Enabled;
			static inline float AR;
			static inline bool AdjustToMods;
			static inline bool AdjustToRate;
			static inline bool DrawPreemptiveDot;
			static inline ImVec4 PreemptiveDotColour;
		};

		struct Removers
		{
			static inline bool HiddenRemoverEnabled;
			static inline bool FlashlightRemoverEnabled;
		};

		struct CSChanger
		{
			static inline bool Enabled;
			static inline float CS;
		};

		struct UI
		{
			static inline int MenuScale;
			static inline char MenuBackground[MAX_PATH];
			static inline bool Snow;
			static inline ImVec4 AccentColour;
			static inline ImVec4 MenuColour;
			static inline ImVec4 ControlColour;
			static inline ImVec4 TextColour;
		};
	};

	struct Misc
	{
		static inline bool DisableSpectators;
		static inline bool DisableScoreSubmission;
		static inline bool DisableLogging;
		static inline bool ForceDisableSubmission = false;

		struct DiscordRichPresenceSpoofer
		{
			static inline bool Enabled;
			static inline bool CustomLargeImageTextEnabled;
			static inline char CustomLargeImageText[128];
			static inline bool CustomPlayModeEnabled;
			static inline int CustomPlayMode;
			static inline bool CustomStateEnabled;
			static inline char CustomState[128];
			static inline bool CustomDetailsEnabled;
			static inline char CustomDetails[128];
			static inline bool HideSpectateButton;
			static inline bool HideMatchButton;
		};
	};
};