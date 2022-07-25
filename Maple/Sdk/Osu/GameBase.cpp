#include "GameBase.h"

#include "../Memory.h"
#include "Utilities/MemoryUtilities.h"

void GameBase::Initialize()
{
	Memory::AddObject("GameBase::Mode", "C3 FF 15 ?? ?? ?? ?? 83 3D", 0x9, 1);
	Memory::AddObject("GameBase::UpdateTiming", "55 8B EC 83 E4 F8 57 56 83 EC 18 8B F9 8B 0D");

	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_1", "GameBase::UpdateTiming", "DD 05 ?? ?? ?? ?? DC 25 ?? ?? ?? ?? D9 C0 DD 05", 0x28E, 0x10, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_2", "GameBase::UpdateTiming", "1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25", 0x28E, 0xD, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_3", "GameBase::UpdateTiming", "DD 05 ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 25", 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
	Memory::AddPatch("GameBase::UpdateTiming_TickratePatch_4", "GameBase::UpdateTiming", "DD 1D ?? ?? ?? ?? DD 05 ?? ?? ?? ?? DC 35", 0x28E, 0xE, MemoryUtilities::IntToByteArray(reinterpret_cast<int>(&tickrate)));
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
