#include "BeatmapManager.h"

#include "../Memory.h"
#include <xorstr.hpp>

BeatmapManager::BeatmapManager(singletonLock)
{ }

void BeatmapManager::Initialize()
{
	Memory::AddObject(xorstr_("BeatmapManager::GetBeatmapByChecksum"), xorstr_("55 8B EC 50 33 C0 89 45 FC 8B D1 85 D2 75"));
	Memory::AddObject(xorstr_("BeatmapManager::set_Current"), xorstr_("55 8B EC 56 8B F1 8B 0D ?? ?? ?? ?? 3B CE 75"));
}

uintptr_t BeatmapManager::GetBeatmapByChecksum(std::wstring beatmapChecksum)
{
	CLRString* hashCLRString = Vanilla::AllocateCLRString(beatmapChecksum.c_str());

	return reinterpret_cast<fnGetBeatmapByChecksum>(Memory::Objects["BeatmapManager::GetBeatmapByChecksum"])(hashCLRString);
}

void BeatmapManager::SetCurrent(uintptr_t beatmapPointer)
{
	reinterpret_cast<fnSetCurrent>(Memory::Objects["BeatmapManager::set_Current"])(beatmapPointer);
}
