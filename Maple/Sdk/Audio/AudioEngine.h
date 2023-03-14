#pragma once

#include <cstdint>

class AudioEngine
{
	static inline constexpr int AUDIO_STATE_OFFSET = 0x30;

	typedef void(__stdcall* fnSetCurrentPlaybackRate)(double rate);
	static inline fnSetCurrentPlaybackRate oSetCurrentPlaybackRate;
	static void __stdcall setCurrentPlaybackRateHook(double rate);
public:
	static void Initialize();
	
	static int GetTime();
	static bool GetIsPaused();
	static double GetModTempo();
	static float GetModFrequency(float currentFrequency);
};
