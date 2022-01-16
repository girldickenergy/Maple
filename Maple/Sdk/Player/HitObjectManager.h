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
	static inline Field spriteDisplaySizeField;
	static inline Field hitObjectRadiusField;
	static inline Field spriteManagerField;
	static inline Field gamefieldSpriteRatioField;
	static inline Field spriteRatioField;
	static inline Field stackOffsetField;
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

	typedef void*(__fastcall* fnGetObject)(void* instance, int index);
	static inline fnGetObject getObject;

	typedef void(__fastcall* fnSetBeatmap)(void* instance, void* beatmap, Mods mods);
	static inline fnSetBeatmap setBeatmap;

	typedef bool(__fastcall* fnLoad)(void* instance, bool processHeaders);
	static inline fnLoad load;
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
	static float GetSpriteDisplaySize();
	static void SetSpriteDisplaySize(float size);
	static float GetHitObjectRadius();
	static void SetHitObjectRadius(float radius);
	static float GetSpriteRatio();
	static void SetSpriteRatio(float ratio);
	static float GetGamefieldSpriteRatio();
	static void SetGamefieldSpriteRatio(float ratio);
	static float GetStackOffset();
	static void SetStackOffset(float offset);
	static int GetCurrentHitObjectIndex();
	static int GetHitObjectsCount();
	static HitObject GetHitObject(int index);
	static std::vector<HitObject> GetAllHitObjects();
	static void SetBeatmap(void* beatmap);
	static bool Load(bool processHeaders);
	static double MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods);
};
