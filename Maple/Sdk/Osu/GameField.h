#pragma once

#include <Vanilla.h>
#include "../DataTypes/Structs/Vector2.h"

class GameField
{
	static inline void* gameFieldInstanceAddress = nullptr;
	
	static inline Field heightField;
	static inline Field widthField;
	static inline Field offsetField;
public:
	static inline TypeExplorer RawGameField;
	static void Initialize();
	static void* Instance();
	static float GetHeight();
	static float GetWidth();
	static float GetRatio();
	static Vector2 GetOffsetVector();
	static Vector2 DisplayToField(Vector2 pos);
	static Vector2 FieldToDisplay(Vector2 pos);
};
