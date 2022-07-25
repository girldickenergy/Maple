#pragma once

#include "Math/Vector2.h"

class InputManager
{
	static inline constexpr int LEFTBUTTON1_OFFSET = 0x27;
	static inline constexpr int LEFTBUTTON1I_OFFSET = 0x5E;
	static inline constexpr int LEFTBUTTON2_OFFSET = 0xCE;
	static inline constexpr int LEFTBUTTON2I_OFFSET = 0x10F;
	static inline constexpr int RIGHTBUTTON1_OFFSET = 0x175;
	static inline constexpr int RIGHTBUTTON1I_OFFSET = 0x1AC;
	static inline constexpr int RIGHTBUTTON2_OFFSET = 0x229;
	static inline constexpr int RIGHTBUTTON2I_OFFSET = 0x260;
	static inline constexpr int LEFTBUTTON_OFFSET = 0x2ED;
	static inline constexpr int RIGHTBUTTON_OFFSET = 0x324;

	static inline Vector2 cursorPosition = Vector2();

	typedef void(__fastcall* fnSetMousePosition)(Vector2 pos);
	static inline fnSetMousePosition oSetMousePosition;
	static void __fastcall setMousePositionHook(Vector2 pos);

	typedef void(__fastcall* fnMouseViaKeyboardControls)();
	static inline fnMouseViaKeyboardControls oMouseViaKeyboardControls;
	static void __fastcall mouseViaKeyboardControlsHook();
public:
	static void Initialize();

	static Vector2 GetCursorPosition();
	static Vector2 Resync(Vector2 displacement, Vector2 offset, float resyncFactor);
};
