#pragma once

#include "Math/Vector2.h"

class GameField
{
	static inline constexpr int WIDTH_OFFSET = 0x8;
	static inline constexpr int HEIGHT_OFFSET = 0xC;
	static inline constexpr int OFFSETVECTOR_OFFSET = 0x18;
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static float GetWidth();
	static float GetHeight();
	static float GetRatio();
	static Vector2 GetOffset();
	static Vector2 DisplayToField(Vector2 display);
	static Vector2 FieldToDisplay(Vector2 field);
};
