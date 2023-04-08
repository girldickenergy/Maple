#pragma once
#include "Singleton.h"
#include <string>
#include "Vanilla.h"

class BeatmapManager : public Singleton<BeatmapManager>
{
	typedef void(__fastcall* fnSetCurrent)(uintptr_t beatmapPointer);
	typedef uintptr_t(__fastcall* fnGetBeatmapByChecksum)(CLRString* beatmapChecksum);
public:
	explicit BeatmapManager(singletonLock);

	void Initialize();
	uintptr_t GetBeatmapByChecksum(std::wstring beatmapChecksum);
	void SetCurrent(uintptr_t beatmapPointer);
};