#pragma once
#include <cstdint>

class SpriteManager
{
	static inline constexpr int GAMEFIELDSPRITERATIO_OFFSET = 0x3C;

	typedef bool(__fastcall* fnDraw)(uintptr_t instance);
public:
	static void Initialize();

	static float GetGamefieldSpriteRatio(uintptr_t instance);
	static void SetGamefieldSpriteRatio(uintptr_t instance, float value);
	static bool Draw(uintptr_t instance);
};
