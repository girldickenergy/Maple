#include "ModManager.h"

#include <Vanilla.h>

void ModManager::Initialize()
{
	RawModManager = Vanilla::Explorer["osu.GameplayElements.Scoring.ModManager"];

	modStatusAddress = RawModManager["ModStatus"].Field.GetAddress();
}

Mods ModManager::CurrentMods()
{
	return *static_cast<Mods*>(modStatusAddress);
}

bool ModManager::IsModEnabled(Mods mod)
{
	return (CurrentMods() & mod) > Mods::None;
}

double ModManager::ModPlaybackRate()
{
	if (IsModEnabled(Mods::HalfTime))
		return 75.;

	if (IsModEnabled(Mods::DoubleTime) || IsModEnabled(Mods::Nightcore))
		return 150.;

	return 100.;
}
