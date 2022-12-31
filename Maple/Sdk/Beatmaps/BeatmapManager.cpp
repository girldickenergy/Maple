#include "BeatmapManager.h"

#include "../Memory.h"
#include "../../Utilities/Security/xorstr.hpp"

BeatmapManager::BeatmapManager(SingletonLock)
{ }

void BeatmapManager::Initialize()
{
	Memory::AddObject(xorstr_("BeatmapManager::Load"), xorstr_("55 8B EC 57 56 53 83 EC 74 8B F1 8D 7D 84 B9 1B 00 00 00 33 C0 F3"));
	Memory::AddObject(xorstr_("BeatmapManager::GetBeatmapByChecksum"), xorstr_("55 8B EC 50 33 C0 89 45 FC 8B D1 85 D2 75"));
}

uintptr_t BeatmapManager::GetBeatmapByChecksum(std::wstring beatmapChecksum)
{
	CLRString* hashCLRString = Vanilla::AllocateCLRString(beatmapChecksum.c_str());

	return reinterpret_cast<fnGetBeatmapByChecksum>(Memory::Objects["BeatmapManager::GetBeatmapByChecksum"])(hashCLRString);
}

void BeatmapManager::Load(void* beatmapPointer)
{
	reinterpret_cast<fnLoad>(Memory::Objects["BeatmapManager::Load"])(beatmapPointer);
}
