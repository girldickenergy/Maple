#include "GameBase.h"

#include "Vanilla.h"
#include "Utilities/MemoryUtilities.h"

#include "../Memory.h"

void GameBase::Initialize()
{
	Memory::AddObject("GameBase::Mode", "C3 FF 15 ?? ?? ?? ?? 83 3D", 0x9, 1);

	Memory::AddObject("GameBase::UpdateTiming", "55 8B EC 83 E4 F8 57 56 83 EC 18 8B F9 8B 0D");
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_1", "GameBase::UpdateTiming", "DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05", 0x28E, 0x10, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_2", "GameBase::UpdateTiming", "1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25", 0x28E, 0xD, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_3", "GameBase::UpdateTiming", "DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25", 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_4", "GameBase::UpdateTiming", "DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35", 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));

	Memory::AddObject("GameBase::ClientHash", "E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 15", 0x12, 1);
	Memory::AddObject("GameBase::UniqueID", "8D 4D D0 E8 ?? ?? ?? ?? 8B 35", 0xA, 1);
	Memory::AddObject("GameBase::UniqueID2", "EB 10 E8 ?? ?? ?? ?? 89 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 1D", 0x14, 1);
	Memory::AddObject("GameBase::UniqueCheck", "C6 40 14 00 8B 1D", 0x6, 1);
}

OsuModes GameBase::GetMode()
{
	const uintptr_t modeAddress = Memory::Objects["GameBase::Mode"];

	return modeAddress ? *reinterpret_cast<OsuModes*>(modeAddress) : OsuModes::Menu;
}

void GameBase::SetTickrate(double value)
{
	tickrate = value;
}

std::wstring GameBase::GetClientHash()
{
	const uintptr_t clientHashAddress = Memory::Objects["GameBase::ClientHash"];

	return clientHashAddress ? (*reinterpret_cast<CLRString**>(clientHashAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueIDInstance()
{
	const uintptr_t uniqueIDAddress = Memory::Objects["GameBase::UniqueID"];

	return uniqueIDAddress ? *reinterpret_cast<uintptr_t*>(uniqueIDAddress) : 0u;
}

std::wstring GameBase::GetUniqueID()
{
	const uintptr_t uniqueIDAddress = Memory::Objects["GameBase::UniqueID"];

	return uniqueIDAddress ? (*reinterpret_cast<CLRString**>(uniqueIDAddress))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueID2Instance()
{
	const uintptr_t uniqueID2Address = Memory::Objects["GameBase::UniqueID2"];

	return uniqueID2Address ? *reinterpret_cast<uintptr_t*>(uniqueID2Address) : 0u;
}

std::wstring GameBase::GetUniqueID2()
{
	const uintptr_t uniqueID2Address = Memory::Objects["GameBase::UniqueID2"];

	return uniqueID2Address ? (*reinterpret_cast<CLRString**>(uniqueID2Address))->Data().data() : L"";
}

uintptr_t GameBase::GetUniqueCheckInstance()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects["GameBase::UniqueCheck"];

	return uniqueCheckAddress ? *reinterpret_cast<uintptr_t*>(uniqueCheckAddress) : 0u;
}

std::wstring GameBase::GetUniqueCheck()
{
	const uintptr_t uniqueCheckAddress = Memory::Objects["GameBase::UniqueCheck"];

	return uniqueCheckAddress ? (*reinterpret_cast<CLRString**>(uniqueCheckAddress))->Data().data() : L"";
}
