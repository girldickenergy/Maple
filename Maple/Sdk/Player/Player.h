#pragma once

#include <cstdint>

#include "Osu/PlayModes.h"

class Player
{
	static inline constexpr int ASYNC_LOAD_COMPLETE_OFFSET = 0x188;
	static inline constexpr int REPLAY_MODE_OFFSET = 0x17F;
	static inline constexpr int BEATMAP_OFFSET = 0xDC;
	static inline constexpr int HIT_OBJECT_MANAGER_OFFSET = 0x48;

	typedef int(__fastcall* fnOnLoadComplete)(uintptr_t instance, bool success);
	static inline fnOnLoadComplete oOnLoadComplete;
	static int __fastcall onLoadCompleteHook(uintptr_t instance, bool success);

	typedef void(__fastcall* fnUpdateFlashlight)(uintptr_t instance);
	static inline fnUpdateFlashlight oUpdateFlashlight;
	static void __fastcall updateFlashlightHook(uintptr_t instance);

	typedef void(__fastcall* fnHandleScoreSubmission)(uintptr_t instance);
	static inline fnHandleScoreSubmission oHandleScoreSubmission;
	static void __fastcall handleScoreSubmissionHook(uintptr_t instance);

	typedef bool(__fastcall* fnOnGamePress)(uintptr_t instance, uintptr_t object, uintptr_t e);
	static inline fnOnGamePress oOnGamePress;
	static bool __fastcall onGamePressHook(uintptr_t instance, uintptr_t object, uintptr_t e);

	typedef void(__fastcall* fnDrawRuleset)(uintptr_t instance);
	static inline fnDrawRuleset oDrawRuleset;
	static void __fastcall drawRulesetHook(uintptr_t instance);

	typedef void(__fastcall* fnDraw)(uintptr_t instance);
public:
	static void Initialize();

	static bool HasInitialized();

	static uintptr_t GetInstance();
	static bool GetIsLoaded();
	static bool GetIsReplayMode();
	static PlayModes GetPlayMode();
	static bool GetIsRetrying();
	static bool GetIsFailed();
	static int GetAnticheatFlag();
	static void ResetAnticheatFlag();
	static uintptr_t GetBeatmapInstance();
	static uintptr_t GetHitObjectManager();
	static void SetHitObjectManager(uintptr_t hitObjectManagerInstance);
	static void DoOnGamePress();
	static void UpdateActive();
	static void Draw();
};
