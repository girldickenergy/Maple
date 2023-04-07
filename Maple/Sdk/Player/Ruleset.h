#pragma once

#include <cstdint>

class Ruleset
{
	static inline constexpr int INSTANCE_OFFSET = 0x68;
	static inline constexpr int BASEMOVEMENTSPEED_OFFSET = 0xB8;

	typedef void(__fastcall* fnLoadFlashlight)(uintptr_t instance);
	static inline fnLoadFlashlight oLoadFlashlight;
	static void __fastcall loadFlashlightHook(uintptr_t instance);

	static inline fnLoadFlashlight oLoadManiaFlashlight;
	static void __fastcall loadManiaFlashlightHook(uintptr_t instance);

	typedef int(__fastcall* fnHasHiddenSprites)(uintptr_t instance);
	static inline fnHasHiddenSprites oHasHiddenSprites;
	static int __fastcall hasHiddenSpritesHook(uintptr_t instance);

	// Replay Editor
	typedef uintptr_t(__fastcall* fnCreateHitObjectManager)(uintptr_t instance);
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static float GetCatcherSpeed();
	static void SetCatcherSpeed(float value);

	// Replay Editor
	static uintptr_t CreateHitObjectManager(uintptr_t instance);
};