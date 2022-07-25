#pragma once

#include "Osu/Mods.h"

class ModManager
{
public:
	static void Initialize();

	static Mods GetActiveMods();
	static bool CheckActive(Mods mods);
	static double GetModPlaybackRate();
};
