#pragma once
#include "COM/COMString.h"

class Timewarp
{
	typedef double(__fastcall* fnGetPlaybackRate)(void* instance);
	typedef void(__fastcall* fnAddParameter)(void* instance, COMString* name, COMString* value);
	
	static inline double tickrate = 1000.0 / 60.0;

	static inline void* audioTrackBass_GetPlaybackRateReturnAddress = nullptr;
	static double __fastcall audioTrackBass_GetPlaybackRateStub(void* instance);
	static inline void* audioTrackVirtual_GetPlaybackRateReturnAddress = nullptr;
	static double __fastcall audioTrackVirtual_GetPlaybackRateStub(void* instance);

	static void patchTickrate();
public:
	static void TimewarpThread();
	static double GetRateMultiplier();

	static inline fnGetPlaybackRate oAudioTrackBass_GetPlaybackRate;
	static double __fastcall AudioTrackBass_GetPlaybackRateHook(void* instance);

	static inline fnGetPlaybackRate oAudioTrackVirtual_GetPlaybackRate;
	static double __fastcall AudioTrackVirtual_GetPlaybackRateHook(void* instance);

	static inline fnAddParameter oAddParameter;
	static void __fastcall AddParameterHook(void* instance, COMString* name, COMString* value);
};
