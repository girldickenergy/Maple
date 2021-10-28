#pragma once

#include <Explorer/TypeExplorer.h>

#include "../DataTypes/Structs/Vector2.h"

class InputManager
{
	static inline void* currentCursorHandlerAddress = nullptr;

	typedef Vector2(__fastcall* fnGetMousePosition)();
	static inline fnGetMousePosition getMousePosition = nullptr;

	typedef void(__fastcall* fnApplyHandler)(void* handler);
	static inline fnApplyHandler applyHandler = nullptr;
public:
	static inline TypeExplorer RawMouseManager;
	static inline TypeExplorer RawInputManager;
	static void Initialize();
	static void* GetCurrentInputHandlerInstance();
	static Vector2 CursorPosition();
	static Vector2 CursorPositionAccurate();
};