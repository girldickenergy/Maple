#pragma once

#include "../../Sdk/Mods/ModManager.h"

class VisualsSpoofers
{
	typedef void(__fastcall* fnParse)(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits);
	typedef void(__fastcall* fnApplyStacking)(void* instance);

	static inline int originalPreEmpt;
	static inline int originalPreEmptSliderComplete;
	static inline Mods originalMods;
	
	static void __fastcall spoofVisuals();
	static void __fastcall restoreVisuals();
public:
	static inline fnParse oParse;
	static void __fastcall ParseHook(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits);

	static inline fnApplyStacking oApplyStacking;
	static void __fastcall ApplyStackingHook(void* instance);
	
	static void FlashlightRemoverThread();
};
