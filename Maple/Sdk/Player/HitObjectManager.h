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
	static inline constexpr int HITOBJECT_STARTTIME_OFFSET = 0x10;
	static inline constexpr int HITOBJECT_ENDTIME_OFFSET = 0x14;
	static inline constexpr int HITOBJECT_POSITION_OFFSET = 0x38;
	static inline constexpr int HITOBJECT_SEGMENTCOUNT_OFFSET = 0x20;
	static inline constexpr int HITOBJECT_STACKCOUNT_OFFSET = 0x2C;
	static inline constexpr int HITOBJECT_SPATIALLENGTH_OFFSET = 0x8;

	static inline constexpr int SLIDEROSU_ENDPOSITION_OFFSET = 0x11C;
	static inline constexpr int SLIDEROSU_SLIDERCURVEPOINTS_OFFSET = 0xC0;
	static inline constexpr int SLIDEROSU_SLIDERCURVESMOOTHLINES_OFFSET = 0xC4;
	static inline constexpr int SLIDEROSU_CUMULATIVELENGTHS_OFFSET = 0xC8;
	static inline constexpr int SLIDEROSU_VELOCITY_OFFSET = 0x9C;
	static inline constexpr int SLIDEROSU_SLIDERSCORETIMINGPOINTS_OFFSET = 0xD8;

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

	// Replay editor
	typedef bool (__fastcall* fnLoad)(uintptr_t instance, bool processHeaders, bool applyParsingLimits);
	typedef void(__fastcall* fnSetBeatmap)(uintptr_t instance, uintptr_t beatmap, Mods mods);
	typedef void(__fastcall* fnUpdateStacking)(uintptr_t instance, int startIndex, int endIndex);
	typedef void(__fastcall* fnUpdateSlidersAll)(uintptr_t instance, bool force);
public:
	static inline std::vector<HitObject> HitObjects;

	static void Initialize();

	static void RestoreVisuals();

	static void CacheHitObjects();
	static void CacheHitObjects(uintptr_t instance);
	static HitObject GetHitObject(int index);

	static uintptr_t GetInstance();
	static int GetPreEmpt(bool original = false);
	static int GetPreEmpt(uintptr_t instance);
	static void SetPreEmpt(int value);
	static int GetPreEmptSliderComplete();
	static void SetPreEmptSliderComplete(int value);
	static Mods GetActiveMods();
	static void SetActiveMods(Mods value);
	static int GetHitWindow50();
	static int GetHitWindow50(uintptr_t instance);
	static int GetHitWindow100();
	static int GetHitWindow100(uintptr_t instance);
	static int GetHitWindow300();
	static int GetHitWindow300(uintptr_t instance);
	static float GetSpriteDisplaySize();
	static void SetSpriteDisplaySize(float value);
	static float GetHitObjectRadius();
	static float GetHitObjectRadius(uintptr_t instance);
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
	static int GetHitObjectsCount(uintptr_t instance);
	static double MapDifficultyRange(double difficulty, double min, double mid, double max, bool adjustToMods);

	// Replay Editor
	static bool Load(uintptr_t instance, bool processHeaders, bool applyParsingLimits);
	static void SetBeatmap(uintptr_t instance, uintptr_t beatmap, Mods mods);
	static void UpdateStacking(uintptr_t instance, int startIndex, int endIndex);
	static void UpdateSlidersAll(uintptr_t instance, bool force);
};
