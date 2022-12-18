#pragma once
#include <cstdint>

class SpriteManager
{
	static inline constexpr int GAMEFIELDSPRITERATIO_OFFSET = 0x3C;
public:
	static float GetGamefieldSpriteRatio(uintptr_t instance);
	static void SetGamefieldSpriteRatio(uintptr_t instance, float value);
};
