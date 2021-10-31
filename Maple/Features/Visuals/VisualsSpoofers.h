#pragma once

#include "../../Sdk/Mods/ModManager.h"

class VisualsSpoofers
{
	typedef void(__fastcall* fnParse)(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits);
	typedef void(__fastcall* fnUpdateStacking)(void* instance, int startIndex, int endIndex);
	typedef void(__fastcall* fnApplyOldStacking)(void* instance);
	typedef void(__fastcall* fnAddFollowPoints)(void* instance, int startIndex, int endIndex);
	
	static void spoofVisuals();

	static inline int originalPreEmpt;
	static inline int originalPreEmptSliderComplete;
	
	static void spoofPreEmpt();
	static void restorePreEmpt();
public:
	static inline fnParse oParse;
	static void __fastcall ParseHook(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits);

	static inline fnUpdateStacking oUpdateStacking;
	static void __fastcall UpdateStackingHook(void* instance, int startIndex, int endIndex);

	static inline fnApplyOldStacking oApplyOldStacking;
	static void __fastcall ApplyOldStackingHook(void* instance);

	static inline fnAddFollowPoints oAddFollowPoints;
	static void __fastcall AddFollowPointsHook(void* instance, int startIndex, int endIndex);
	
	static void FlashlightRemoverThread();
};
