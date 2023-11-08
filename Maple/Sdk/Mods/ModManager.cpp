#include "ModManager.h"

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Communication/Communication.h"

void ModManager::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("ModManager::ModStatus"), xorstr_("53 8B F1 A1 ?? ?? ?? ?? 25 ?? ?? ?? ?? 85 C0 0F 9F C0 0F B6 C0 8B F8 80 3D"), 0x4, 1);

	VIRTUALIZER_FISH_RED_END
}

Mods ModManager::GetActiveMods()
{
	const uintptr_t modStatusAddress = Memory::Objects[xorstr_("ModManager::ModStatus")];

	return modStatusAddress ? *reinterpret_cast<Mods*>(modStatusAddress) : Mods::None;
}

bool ModManager::CheckActive(Mods mods)
{
	return (GetActiveMods() & mods) > Mods::None;
}

double ModManager::GetModPlaybackRate()
{
	if (CheckActive(Mods::HalfTime))
		return 75.;

	if (CheckActive(Mods::DoubleTime) || CheckActive(Mods::Nightcore))
		return 150.;

	return 100.;
}
