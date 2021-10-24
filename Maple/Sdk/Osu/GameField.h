#pragma once

#include <Vanilla.h>
#include "../DataTypes/Structs/Vector2.h"

class GameField
{
	static inline Field gameFieldField;

	typedef float(__thiscall* fnDisplayToFieldX)(void* instance, float x);
	typedef float(__thiscall* fnDisplayToFieldY)(void* instance, float y);

	typedef float(__thiscall* fnFieldToDisplayX)(void* instance, float x);
	typedef float(__thiscall* fnFieldToDisplayY)(void* instance, float y);

	static inline fnDisplayToFieldX displayToFieldX;
	static inline fnDisplayToFieldY displayToFieldY;

	static inline fnFieldToDisplayX fieldToDisplayX;
	static inline fnFieldToDisplayY fieldToDisplayY;
public:
	static inline TypeExplorer RawGameField;
	static void Initialize();
	static void* Instance();
	static Vector2 DisplayToField(Vector2 pos);
	static Vector2 FieldToDisplay(Vector2 pos);
};
