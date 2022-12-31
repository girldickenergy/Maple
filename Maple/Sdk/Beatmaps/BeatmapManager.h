#pragma once
#include "../../Utilities/Architecture/Singleton.hpp"
#include <string>
#include "Vanilla.h"

class BeatmapManager : public Singleton<BeatmapManager>
{
	typedef void(__fastcall* fnLoad)(void* beatmapPointer);
	typedef uintptr_t(__fastcall* fnGetBeatmapByChecksum)(CLRString* beatmapChecksum);
public:
	explicit BeatmapManager(SingletonLock);

	void Initialize();
	uintptr_t GetBeatmapByChecksum(std::wstring beatmapChecksum);
	void Load(void* beatmapPointer);
};