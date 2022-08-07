#include "ModManager.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Utilities/Security/xorstr.hpp"

void ModManager::Initialize()
{
	STR_ENCRYPT_START

	Memory::AddObject(xor ("ModManager::ModStatus"), xor ("53 8B F1 A1"), 0x4, 1);

	STR_ENCRYPT_END
}

Mods ModManager::GetActiveMods()
{
	const uintptr_t modStatusAddress = Memory::Objects[xor ("ModManager::ModStatus")];

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
