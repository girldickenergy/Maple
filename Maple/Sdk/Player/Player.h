#pragma once

#include <Explorer/TypeExplorer.h>
#include <Enums/PlayModes.h>

class Player
{
	static inline bool isLoaded = false;
	
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

	typedef void(__fastcall* fnDispose)(void* instance, BOOL disposing);
	static inline fnDispose oDispose;
	static void __fastcall DisposeHook(void* instance, BOOL disposing);

	typedef BOOL(__fastcall* fnOnPlayerLoadComplete)(void* instance, BOOL success);
	static inline fnOnPlayerLoadComplete oOnPlayerLoadComplete;
	static BOOL __fastcall OnPlayerLoadCompleteHook(void* instance, BOOL success);
};
