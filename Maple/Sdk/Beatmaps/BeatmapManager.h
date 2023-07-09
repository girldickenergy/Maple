#pragma once
#include "Singleton.h"
#include "Vanilla.h"
#include <string>

class BeatmapManager : public Singleton<BeatmapManager>
{
    typedef void(__fastcall* fnSetCurrent)(uintptr_t beatmapPointer);
    typedef uintptr_t(__fastcall* fnGetBeatmapByChecksum)(CLRString* beatmapChecksum);

public:
    explicit BeatmapManager(singletonLock);

    void Initialize();
    uintptr_t GetBeatmapByChecksum(std::string beatmapChecksum);
    void SetCurrent(uintptr_t beatmapPointer);
};