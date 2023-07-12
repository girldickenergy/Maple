#include "BeatmapManager.h"

#include <xorstr.hpp>

#include "../Memory.h"

BeatmapManager::BeatmapManager(singletonLock) {}

void BeatmapManager::Initialize()
{
    Memory::AddObject(xorstr_("BeatmapManager::GetBeatmapByChecksum"), xorstr_("55 8B EC 50 33 C0 89 45 FC 8B D1 85 D2 75"));
    Memory::AddObject(xorstr_("BeatmapManager::set_Current"), xorstr_("55 8B EC 56 8B F1 8B 0D ?? ?? ?? ?? 3B CE 75"));

    // Taken from osu.GameBase.loadQueuedMode
    Memory::AddObject(xorstr_("BeatmapManager::Beatmaps"), xorstr_("83 3D ?? ?? ?? ?? 13 74 49 A1"), 0xA);
}

uintptr_t BeatmapManager::GetBeatmapByChecksum(std::string beatmapChecksum)
{
    const auto beatmapsPointer = **reinterpret_cast<uintptr_t**>(Memory::Objects[xorstr_("BeatmapManager::Beatmaps")]);
    const int32_t beatmapAmount = *reinterpret_cast<int32_t*>(beatmapsPointer + 0xC);

    const auto beatmapsList = *reinterpret_cast<uintptr_t*>(beatmapsPointer + 0x04);
    for (int i = 0; i < beatmapAmount; i++)
    {
        const uintptr_t beatmapPointer = *reinterpret_cast<uintptr_t*>(beatmapsList + 0x08 + 0x04 * i);
        const uintptr_t beatmapHashPointer = *reinterpret_cast<uintptr_t*>(beatmapPointer + 0x6C);

        // For some reason I've had (some) beatmaps with no hash
        if (beatmapHashPointer == NULL)
            continue;

        const int32_t hashLength = *reinterpret_cast<int32_t*>(beatmapHashPointer + 0x04);
        std::string hash;
        for (int j = 0; j < hashLength; j++)
        {
            const char8_t currentChar = *reinterpret_cast<char8_t*>(beatmapHashPointer + 0x08 + (j * 2));
            hash += currentChar;
        }

        if (beatmapChecksum == hash)
            return beatmapPointer;
    }

    return NULL;
}

void BeatmapManager::SetCurrent(uintptr_t beatmapPointer)
{
    reinterpret_cast<fnSetCurrent>(Memory::Objects[xorstr_("BeatmapManager::set_Current")])(beatmapPointer);
}