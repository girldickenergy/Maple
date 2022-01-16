#pragma once

#include <comdef.h>
#include <string>
#include <vector>

#include "imgui.h"

class Config
{
	static bool isSameName(const std::string& a, const std::string& b);
	static bool isValidName(std::string name);
	static ImVec4 parseImVec4(std::string vec);
	static void loadDefaults();
public:
	static inline std::vector<std::string> Configs;
	static inline int CurrentConfig = 0;
	static inline char NewConfigName[20] = { };

	static void Initialize();
	static void Load();
	static void Save();
	static void Create();
	static void Refresh();

	struct Relax
	{
		static inline bool Enabled;
		static inline int Distribution;
		static inline int Playstyle;
		static inline int PrimaryKey;
		static inline int SecondaryKey;
		static inline int MaxSingletapBPM;
		static inline int HitSpread;
		static inline int AlternationHitSpread;
		static inline bool HoldConsecutiveSpinners;
		static inline bool SliderAlternationOverride;
		static inline bool PredictionEnabled;
		static inline bool SliderPredictionEnabled;
		static inline int PredictionAngle;
		static inline float PredictionScale;
		static inline bool UseLowestPossibleHoldTimes;
	};
	
	struct AimAssist
	{
		static inline bool Enabled;
		static inline int Algorithm;
		static inline bool EasyMode;
		/* Easy mode arguments*/
		static inline float EasyModeStrength;
		/* Advanced mode arguments*/
		static inline float Strength;
		static inline int BaseFOV;
		static inline float MaximumFOVScale;
		static inline float MinimumFOVTotal;
		static inline float MaximumFOVTotal;
		static inline float SliderballDeadzone;
		static inline bool FlipSliderballDeadzone;
		static inline float AssistDeadzone;
		static inline float StrengthMultiplier;
		static inline float ResyncLeniency;
		static inline float ResyncLeniencyFactor;
		static inline bool AssistOnSliders;
		static inline bool DrawDebugOverlay;
		static inline float Algorithmv2Power;
		static inline bool Algorithmv2AssistOnSliders;
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
		static inline bool ARChangerEnabled;
		static inline float AR;
		static inline bool ARChangerAdjustToMods;
		static inline bool ARChangerAdjustToRate;
		static inline bool ARChangerDrawPreemptiveDot;
		static inline ImVec4 ARChangerPreemptiveDotColour;
		static inline bool HiddenDisabled;
		static inline bool FlashlightDisabled;
		static inline bool CSChangerEnabled;
		static inline float CS;
		static inline int MenuScale;
		static inline char MenuBackground[MAX_PATH];
		static inline ImVec4 AccentColour;
		static inline ImVec4 MenuColour;
		static inline ImVec4 ControlColour;
		static inline ImVec4 TextColour;
	};

	struct Misc
	{
		static inline bool DisableSpectators;
		static inline bool PromptOnScoreSubmissionEnabled;
		static inline bool DisableLogging;
		static inline bool RichPresenceSpooferEnabled;
		static inline char SpoofedName[64];
		static inline char SpoofedRank[64];
	};
};
