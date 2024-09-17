#pragma once

#include "CLR/CLRString.h"
#include "Math/Vector2.h"
#include "Osu/OsuModes.h"

struct __attribute__((__packed__)) Stopwatch
{
	uintptr_t VTable;
	long long Elapsed;
	long long StartTimeStamp;
	bool IsRunning;
};

class GameBase
{
	static inline Stopwatch** stopwatchPtr = 0u;

	static inline bool stopwatchInitialized = false;
	static inline long long stopwatchCurrent = 0;
	static inline long long stopwatchPrevious = 0;

	typedef double(__fastcall* fnGetElapsedMillisecondsPrecise)(Stopwatch* instance);
	static inline fnGetElapsedMillisecondsPrecise oGetElapsedMillisecondsPrecise;
	static double __fastcall GetElapsedMillisecondsPreciseHook(Stopwatch* instance);

	static inline double tickrate = 1.0;
public:
	static void Initialize();

	static int GetTime();
	static OsuModes GetMode();
	static void SetTickrate(double value);
	static bool GetIsFullscreen();
	static Vector2 GetClientSize();
	static Vector2 GetClientPosition();
	static std::wstring GetClientHash();
	static uintptr_t GetUniqueIDInstance();
	static std::wstring GetUniqueID();
	static uintptr_t GetUniqueID2Instance();
	static std::wstring GetUniqueID2();
	static uintptr_t GetUniqueCheckInstance();
	static std::wstring GetUniqueCheck();
};
