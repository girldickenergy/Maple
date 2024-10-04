#pragma once

#include "CLR/CLRString.h"
#include "Math/Vector2.h"
#include "Osu/OsuModes.h"

class GameBase
{
	static inline double tickrate = 1000.0 / 60.0;;
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
