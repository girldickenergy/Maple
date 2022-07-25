#pragma once

#include <cstdint>

#include "Osu/PlayModes.h"

class Player
{
	static inline constexpr int ASYNC_LOAD_COMPLETE_OFFSET = 0x182;
	static inline constexpr int PLAY_MODE_OFFSET = 0x114;
	static inline constexpr int REPLAY_MODE_OFFSET = 0x17A;

	typedef bool(__fastcall* fnOnLoadComplete)(void* instance, bool success);
	static inline fnOnLoadComplete oOnLoadComplete;
	static bool __fastcall onLoadCompleteHook(void* instance, bool success);
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static bool GetIsLoaded();
	static bool GetIsReplayMode();
	static PlayModes GetPlayMode();
	static bool GetIsRetrying();
	static int GetAnticheatFlag();
	static void ResetAnticheatFlag();
};
