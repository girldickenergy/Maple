#include "ModManager.h"

#include "../Memory.h"

void ModManager::Initialize()
{
	Memory::AddObject("ModManager::ModStatus", "53 8B F1 A1", 0x4, 1);
}

Mods ModManager::GetActiveMods()
{
	const uintptr_t modStatusAddress = Memory::Objects["ModManager::ModStatus"];

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
