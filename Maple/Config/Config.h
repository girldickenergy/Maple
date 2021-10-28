#pragma once

#include <comdef.h>
#include <string>
#include <vector>

#include "imgui.h"

class Config
{
	static void ensureDirectoryExists();
	static bool isSameName(const std::string& a, const std::string& b);
	static bool isValidName(std::string name);
	static ImVec4 parseImVec4(std::string vec);
	static void loadDefaults();
public:
	static inline std::string Directory;
	static inline std::vector<std::string> Configs;
	static inline int CurrentConfig = 0;
	static inline char NewConfigName[20] = { };

	static void Initialize(const std::string& directoryName);
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
		static inline bool AccurateCalculations;
		static inline bool PredictionEnabled;
		static inline bool SliderPredictionEnabled;
		static inline int PredictionAngle;
		static inline float PredictionScale;
	};
	
	struct AimAssist
	{
		static inline bool Enabled;
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
	};

	struct Timewarp
	{
		static inline bool Enabled;
		static inline int Rate;
	};

	struct Visuals
	{
		static inline bool ARChangerEnabled;
		static inline bool ARChangerAdjustToMods;
		static inline float AR;
		static inline bool HiddenDisabled;
		static inline bool FlashlightDisabled;
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
		static inline bool RichPresenceSpooferEnabled;
		static inline char SpoofedName[64];
		static inline char SpoofedRank[64];
	};
};
