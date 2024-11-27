#pragma once

#include <cstdint>

#include "HitObject.h"
#include "Osu/Mods.h"

class HitObjectManager
{
	static inline std::vector<HitObject> hitObjects;

	static inline constexpr int HITOBJECTMANAGER_INSTANCE_OFFSET = 0x48;

	static inline constexpr int HITOBJECTMANAGER_PREEMPT_OFFSET = 0x1C;
	static inline constexpr int HITOBJECTMANAGER_PREEMPTSLIDERCOMPLETE_OFFSET = 0x80;
	static inline constexpr int HITOBJECTMANAGER_ACTIVEMODS_OFFSET = 0x34;
	static inline constexpr int HITOBJECTMANAGER_HITWINDOW50_OFFSET = 0x20;
	static inline constexpr int HITOBJECTMANAGER_HITWINDOW100_OFFSET = 0x24;
	static inline constexpr int HITOBJECTMANAGER_HITWINDOW300_OFFSET = 0x28;
	static inline constexpr int HITOBJECTMANAGER_SPRITEDISPLAYSIZE_OFFSET = 0x7C;
	static inline constexpr int HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET = 0x18;
	static inline constexpr int HITOBJECTMANAGER_SPRITERATIO_OFFSET = 0x78;
	static inline constexpr int HITOBJECTMANAGER_SPRITEMANAGER_OFFSET = 0x58;
	static inline constexpr int HITOBJECTMANAGER_STACKOFFSET_OFFSET = 0x2C;
	static inline constexpr int HITOBJECTMANAGER_CURRENTHITOBJECTINDEX_OFFSET = 0x8C;
	static inline constexpr int HITOBJECTMANAGER_HITOBJECTSCOUNT_OFFSET = 0x90;
	static inline constexpr int HITOBJECTMANAGER_HITOBJECTS_OFFSET = 0x48;

	static inline constexpr int HITOBJECT_TYPE_OFFSET = 0x18;
	static inline constexpr int HITOBJECT_HIT_SOUND_TYPE_OFFSET = 0x1C;
	static inline constexpr int HITOBJECT_STARTTIME_OFFSET = 0x10;
	static inline constexpr int HITOBJECT_ENDTIME_OFFSET = 0x14;
	static inline constexpr int HITOBJECT_POSITION_OFFSET = 0x38;
	static inline constexpr int HITOBJECT_SEGMENTCOUNT_OFFSET = 0x20;
	static inline constexpr int HITOBJECT_SPATIALLENGTH_OFFSET = 0x8;

	static inline constexpr int HITOBJECT_MIN_HIT_DELAY_OFFSET = 0x160; // taiko
	static inline constexpr int HITOBJECT_ROTATION_REQUIREMENT_OFFSET = 0xF8; // osu & taiko

	static inline constexpr int SLIDEROSU_ENDPOSITION_OFFSET = 0x140;
	static inline constexpr int SLIDEROSU_SLIDERCURVEPOINTS_OFFSET = 0xD4;
	static inline constexpr int SLIDEROSU_SLIDERCURVESMOOTHLINES_OFFSET = 0xD8;
	static inline constexpr int SLIDEROSU_CUMULATIVELENGTHS_OFFSET = 0xDC;

	static inline int originalPreEmpt;
	static inline int originalPreEmptSliderComplete;
	static inline Mods originalMods;
	static void spoofVisuals();
	static void spoofMods();
	static void spoofPreEmpt();
	static void restoreMods();
	static void restorePreEmpt();
	
	typedef void(__fastcall* fnParse)(uintptr_t instance, int sectionsToParse, bool updateChecksum, bool applyParsingLimits);
	static inline fnParse oParse;
	static void __fastcall parseHook(uintptr_t instance, int sectionsToParse, bool updateChecksum, bool applyParsingLimits);

	typedef void(__fastcall* fnUpdateStacking)(uintptr_t instance, int startIndex, int endIndex);
	static inline fnUpdateStacking oUpdateStacking;
	static void __fastcall updateStackingHook(uintptr_t instance, int startIndex, int endIndex);

	typedef void(__fastcall* fnApplyOldStacking)(uintptr_t instance);
	static inline fnApplyOldStacking oApplyOldStacking;
	static void __fastcall applyOldStackingHook(uintptr_t instance);

	typedef void(__fastcall* fnAddFollowPoints)(uintptr_t instance, int startIndex, int endIndex);
	static inline fnAddFollowPoints oAddFollowPoints;
	static void __fastcall addFollowPointsHook(uintptr_t instance, int startIndex, int endIndex);
public:
	static void Initialize();

	static void RestoreVisuals();

	static void CacheHitObjects();
	static HitObject GetHitObject(int index);

	static uintptr_t GetInstance();
	static int GetPreEmpt(bool original = false);
	static void SetPreEmpt(int value);
	static int GetPreEmptSliderComplete();
	static void SetPreEmptSliderComplete(int value);
	static Mods GetActiveMods();
	static void SetActiveMods(Mods value);
	static int GetHitWindow50();
	static int GetHitWindow100();
	static int GetHitWindow300();
	static float GetSpriteDisplaySize();
	static void SetSpriteDisplaySize(float value);
	static float GetHitObjectRadius();
	static void SetHitObjectRadius(float value);
	static float GetSpriteRatio();
	static void SetSpriteRatio(float value);
	static uintptr_t GetSpriteManagerInstance();
	static float GetGamefieldSpriteRatio();
	static void SetGamefieldSpriteRatio(float value);
	static float GetStackOffset();
	static void SetStackOffset(float value);
	static int GetCurrentHitObjectIndex();
	static int GetHitObjectsCount();
	static double MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods);
};
