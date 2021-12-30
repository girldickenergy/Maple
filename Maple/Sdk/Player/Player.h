#pragma once

#include <Explorer/TypeExplorer.h>

enum class PlayModes : int
{
	Osu = 0,
	Taiko = 1,
	Catch = 2,
	Mania = 3
};

class Player
{
	static inline bool loadComplete = false;
	
	static inline Field asyncLoadCompleteField;
	static inline Field replayModeStableField;
	
	static inline void* instanceAddress = nullptr;
	static inline void* isRetryingAddress = nullptr;
	static inline void* playingAddress = nullptr;
	static inline void* modeAddress = nullptr;
	static inline void* pausedAddress = nullptr;
public:
	static inline TypeExplorer RawPlayer;
	
	static void Initialize();
	static void* Instance();
	static bool IsLoaded();
	static bool IsReplayMode();
	static bool IsRetrying();
	static bool IsPlaying();
	static PlayModes PlayMode();
	static bool IsPaused();

	typedef void(__fastcall* fnPlayerInitialize)(uintptr_t instance);
	static inline fnPlayerInitialize oPlayerInitialize;
	static void __fastcall PlayerInitialize(uintptr_t instance);

	typedef BOOL(__fastcall* fnOnPlayerLoadComplete)(void* instance, BOOL success);
	static inline fnOnPlayerLoadComplete oOnPlayerLoadComplete;
	static BOOL __fastcall OnPlayerLoadCompleteHook(void* instance, BOOL success);
};
