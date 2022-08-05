#pragma once

#include <cstdint>

#include "Osu/Mods.h"

class HitObjectManager
{
	static inline constexpr int HITOBJECTMANAGER_INSTANCE_OFFSET = 0x40;

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
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static int GetPreEmpt();
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
	static void GetSpriteRatio(float value);
	static uintptr_t GetSpriteManagerInstance();
	static float GetGamefieldSpriteRatio();
	static void SetGamefieldSpriteRatio(float value);
	static float GetStackOffset();
	static void SetStackOffset(float value);
	static int GetCurrentHitObjectIndex();
	static int GetHitObjectsCount();
};
