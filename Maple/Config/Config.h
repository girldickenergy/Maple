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

		struct Prediction
		{
			static inline bool Enabled;
			static inline bool SliderPredictionEnabled;
			static inline int Angle;
			static inline float Scale;
		};

		struct Blatant
		{
			static inline bool UseLowestPossibleHoldTimes;
		};
	};
	
	struct AimAssist
	{
		static inline bool Enabled;
		static inline int Algorithm;

		struct Algorithmv1
		{
			struct EasyMode
			{
				static inline bool Enabled;
				static inline float Strength;
			};

			struct AdvancedMode
			{
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
			};
		};
		
		struct Algorithmv2
		{
			static inline float Power;
			static inline bool AssistOnSliders;
		};

		struct Algorithmv3
		{
			static inline float Strength;
			static inline bool AssistOnSliders;
			static inline int BaseFOV;
			static inline float MaximumFOVScale;
			static inline float MinimumFOVTotal;
			static inline float MaximumFOVTotal;
			static inline float AccelerationFactor;
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
		static inline bool PromptOnScoreSubmissionEnabled;
		static inline bool DisableLogging;
		static inline bool DisableSubmission = false;

		struct RichPresenceSpoofer
		{
			static inline bool Enabled;
			static inline char Name[64];
			static inline char Rank[64];
		};
	};
};
