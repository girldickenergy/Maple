#pragma once

#include "../../Sdk/Mods/ModManager.h"

class VisualsSpoofers
{
	typedef void(__fastcall* fnParse)(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits);
	typedef void(__fastcall* fnUpdateStacking)(void* instance, int startIndex, int endIndex);
	typedef void(__fastcall* fnApplyOldStacking)(void* instance);
	typedef void(__fastcall* fnAddFollowPoints)(void* instance, int startIndex, int endIndex);
	typedef void(__fastcall* fnLoadFlashlight)(void* instance);
	typedef void(__fastcall* fnUpdateFlashlight)(void* instance);
	typedef BOOL(__fastcall* fnHasHiddenSprites)(void* instance);
	
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
	
	static inline fnLoadFlashlight oLoadFlashlight;
	static void __fastcall LoadFlashlightHook(void* instance);

	static inline fnLoadFlashlight oLoadFlashlightMania;
	static void __fastcall LoadFlashlightManiaHook(void* instance);

	static inline fnUpdateFlashlight oUpdateFlashlight;
	static void __fastcall UpdateFlashlightHook(void* instance);

	static inline fnHasHiddenSprites oHasHiddenSprites;
	static BOOL __fastcall HasHiddenSpritesHook(void* instance);
};
