#pragma once

#include <cstdint>

class Ruleset
{
	static inline constexpr int INSTANCE_OFFSET = 0x60;
	static inline constexpr int BASEMOVEMENTSPEED_OFFSET = 0xB8;
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static float GetCatcherSpeed();
	static void SetCatcherSpeed(float value);
};