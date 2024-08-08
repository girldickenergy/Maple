#pragma once

#include <cstdint>

#include "Osu/PlayModes.h"

class Player
{
	static inline constexpr int ASYNC_LOAD_COMPLETE_OFFSET = 0x188;
	static inline constexpr int REPLAY_MODE_OFFSET = 0x17F;

	typedef int(__fastcall* fnOnLoadComplete)(uintptr_t instance, bool success);
	static inline fnOnLoadComplete oOnLoadComplete;
	static int __fastcall onLoadCompleteHook(uintptr_t instance, bool success);

	typedef void(__fastcall* fnUpdateFlashlight)(uintptr_t instance);
	static inline fnUpdateFlashlight oUpdateFlashlight;
	static void __fastcall updateFlashlightHook(uintptr_t instance);

	typedef void(__fastcall* fnHandleScoreSubmission)(uintptr_t instance);
	static inline fnHandleScoreSubmission oHandleScoreSubmission;
	static void __fastcall handleScoreSubmissionHook(uintptr_t instance);

	static inline bool modePtrChecked = false;
	static inline PlayModes* modePtr = nullptr;
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static bool GetIsLoaded();
	static bool GetIsReplayMode();
	static PlayModes GetPlayMode();
	static bool GetIsRetrying();
	static bool GetIsFailed();
	static int GetAnticheatFlag();
	static void ResetAnticheatFlag();
};
