#pragma once

#include <Explorer/TypeExplorer.h>

#include "../Mods/ModManager.h"
#include "HitObject.h"

class HitObjectManager
{
	static inline Field hitObjectManagerField;
	static inline Field preEmptField;
	static inline Field preEmptSliderCompleteField;
	static inline Field activeModsField;
	static inline Field hitWindow300Field;
	static inline Field hitWindow100Field;
	static inline Field hitWindow50Field;
	static inline Field hitObjectRadiusField;
	static inline Field currentHitObjectIndexField;
	static inline Field hitObjectsCountField;
	/* HitObject + HitObjectSliderOsu specific fields | Maybe move these into a subclass*/
	static inline Field hitObjectTypeField;
	static inline Field hitObjectStartTimeField;
	static inline Field hitObjectEndTimeField;
	static inline Field hitObjectPositionField;
	static inline Field hitObjectEndPositionField;
	static inline Field hitObjectSegmentCountField;
	static inline Field hitObjectSpatialLengthField;
	static inline Field hitObjectSliderCurvePointsField;
	static inline Field hitObjectCumulativeLengthsField;
	static inline Field hitObjectSliderCurveSmoothLinesField;
	
	static inline TypeExplorer obfuscatedType;

	typedef Mods(__fastcall* fnObfuscatedGetValue)(void* instance);
	static inline fnObfuscatedGetValue obfuscatedGetValue;
	
	typedef void(__fastcall* fnObfuscatedSetValue)(void* instance, Mods value);
	static inline fnObfuscatedSetValue obfuscatedSetValue;

	typedef uintptr_t(__fastcall* fnGetObject)(int index);
	static inline fnGetObject getObject;
public:
	static inline TypeExplorer RawHitObjectManager;
	static inline TypeExplorer RawHitObject;
	static inline TypeExplorer RawHitObjectSliderOsu;
	
	static void Initialize();
	static void* Instance();
	static int GetPreEmpt();
	static void SetPreEmpt(int preEmpt);
	static int GetPreEmptSliderComplete();
	static void SetPreEmptSliderComplete(int preEmptSliderComplete);
	static Mods GetActiveMods();
	static void SetActiveMods(Mods mods);
	static int GetHitWindow300();
	static int GetHitWindow100();
	static int GetHitWindow50();
	static float GetHitObjectRadius();
	static int GetCurrentHitObjectIndex();
	static int GetHitObjectsCount();
	HitObject GetHitObject(int index);
	std::vector<HitObject> GetAllHitObjects();
	static double MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods);
};
