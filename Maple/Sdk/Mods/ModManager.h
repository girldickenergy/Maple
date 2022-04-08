#pragma once

#include <Explorer/TypeExplorer.h>
#include <Enums/Mods.h>

class ModManager
{
	static inline void* modStatusAddress = nullptr;
public:
	static inline TypeExplorer RawModManager;
	
	static void Initialize();
	static Mods CurrentMods();
	static bool IsModEnabled(Mods mod);
	static double ModPlaybackRate();
};