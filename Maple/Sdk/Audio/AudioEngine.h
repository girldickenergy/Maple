#pragma once

#include <cstdint>
#include <mutex>

class AudioEngine
{
	static inline constexpr int AUDIO_STATE_OFFSET = 0x30;

	static inline std::mutex mutex;

	typedef void(__stdcall* fnSetCurrentPlaybackRate)(double rate);
	static inline fnSetCurrentPlaybackRate oSetCurrentPlaybackRate;
	static void __stdcall setCurrentPlaybackRateHook(double rate);

	// Replay Editor
	typedef bool(__stdcall* fnTogglePause)();
	typedef void(__stdcall* fnSeekTo)(int milliseconds, bool allowExceedingRange, bool force);
	typedef bool(__stdcall* fnLoadAudio)(uintptr_t beatmapPointer, bool requireId3, bool quick, bool unloadPrevious, bool loop);
public:
	static void Initialize();
	
	static int GetTime();
	static bool GetIsPaused();
	static double GetModTempo();
	static float GetModFrequency(float currentFrequency);

	// Replay Editor
	static bool TogglePause();
	/**
	 * \brief WARNING! Calling this function will call the ORIGINAL function, therefore the hook for `SetCurrentPlaybackRate` will not be hit!
	 */
	static void SetCurrentPlaybackRate(double rate);
	static void SeekTo(int milliseconds, bool allowExceedingRange, bool force);
	static bool LoadAudio(uintptr_t beatmapPointer, bool requireId3, bool quick, bool unloadPrevious, bool loop);
};
