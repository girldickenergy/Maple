#pragma once

class AudioEngine
{
	static inline constexpr int AUDIO_STATE_OFFSET = 0x30;

	typedef double(__fastcall* fnGetCurrentPlaybackRate)();
	static inline fnGetCurrentPlaybackRate oGetCurrentPlaybackRate;
	static double __fastcall getCurrentPlaybackRateHook();

	typedef void(__fastcall* fnSetCurrentPlaybackRate)(double rate);
	static inline fnSetCurrentPlaybackRate oSetCurrentPlaybackRate;
	static void __fastcall setCurrentPlaybackRateHook(double rate);

	static inline void* getCurrentPlaybackRateReturnAddress = nullptr;
	static double __fastcall getCurrentPlaybackRateStub();
public:
	static void Initialize();
	
	static int GetTime();
	static bool GetIsPaused();
};