#pragma once

#include <Explorer/TypeExplorer.h>

#include "../DataTypes/Structs/Vector2.h"

class InputManager
{
	typedef Vector2(__fastcall* fnGetMousePosition)();
	static inline fnGetMousePosition getMousePosition = nullptr;
public:
	static inline TypeExplorer RawInputManager;
	static inline TypeExplorer RawMouseManager;
	static void Initialize();
	static Vector2 CursorPosition();
};