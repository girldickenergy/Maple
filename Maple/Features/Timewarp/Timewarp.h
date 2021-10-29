#pragma once

#include <COM/COMString.h>

class Timewarp
{
	typedef double(__fastcall* fnGetCurrentPlaybackRate)();
	typedef void(__fastcall* fnAddParameter)(void* instance, COMString* name, COMString* value);
	
	static inline double tickrate = 1000.0 / 60.0;

	static inline void* getCurrentPlaybackRateReturnAddress = nullptr;
	static double __fastcall getCurrentPlaybackRateStub();

	static void patchTickrate();
public:
	static void TimewarpThread();
	static double GetRateMultiplier();

	static inline fnGetCurrentPlaybackRate oGetCurrentPlaybackRate;
	static double __fastcall GetCurrentPlaybackRateHook();

	static inline fnAddParameter oAddParameter;
	static void __fastcall AddParameterHook(void* instance, COMString* name, COMString* value);
};
