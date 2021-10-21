#pragma once

#include <Explorer/TypeExplorer.h>

#include "../Mods/ModManager.h"

class HitObjectManager
{
	static inline Field hitObjectManagerField;
	static inline Field preEmptField;
	static inline Field preEmptSliderCompleteField;
	static inline Field activeModsField;
	
	static inline TypeExplorer obfuscatedType;

	typedef Mods(__fastcall* fnObfuscatedGetValue)(void* instance);
	static inline fnObfuscatedGetValue obfuscatedGetValue;
	
	typedef void(__fastcall* fnObfuscatedSetValue)(void* instance, Mods value);
	static inline fnObfuscatedSetValue obfuscatedSetValue;
public:
	static inline TypeExplorer RawHitObjectManager;
	
	static void Initialize();
	static void* Instance();
	static int GetPreEmpt();
	static void SetPreEmpt(int preEmpt);
	static int GetPreEmptSliderComplete();
	static void SetPreEmptSliderComplete(int preEmptSliderComplete);
	static Mods GetActiveMods();
	static void SetActiveMods(Mods mods);
	static double MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods);
};
