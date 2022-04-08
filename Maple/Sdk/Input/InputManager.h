#pragma once

#include <Explorer/TypeExplorer.h>

#include "../DataTypes/Structs/Vector2.h"

class InputManager
{
	typedef Vector2(__fastcall* fnGetMousePosition)();
	static inline fnGetMousePosition getMousePosition = nullptr;

	static inline void* leftButton1Address = nullptr;
	static inline void* leftButton1iAddress = nullptr;
	static inline void* leftButton2Address = nullptr;
	static inline void* leftButton2iAddress = nullptr;
	static inline void* rightButton1Address = nullptr;
	static inline void* rightButton1iAddress = nullptr;
	static inline void* rightButton2Address = nullptr;
	static inline void* rightButton2iAddress = nullptr;
public:
	static inline TypeExplorer RawInputManager;
	static inline TypeExplorer RawMouseManager;
	static void Initialize();
	static Vector2 CursorPosition();

	typedef void(__stdcall* fnSetMousePosition)(float x, float y);
	static inline fnSetMousePosition oSetMousePosition;
	static void __stdcall SetMousePositionHook(float x, float y);

	typedef void(__fastcall* fnMouseViaKeyboardControls)();
	static inline fnMouseViaKeyboardControls oMouseViaKeyboardControls;
	static void __fastcall MouseViaKeyboardControlsHook();
};