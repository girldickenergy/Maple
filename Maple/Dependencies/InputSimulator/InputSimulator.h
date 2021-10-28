#pragma once

#include <windows.h>

class InputSimulator
{
public:
	static void KeyDown(WORD keyCode);
	static void KeyUp(WORD keyCode);
	static void LeftMouseButtonDown();
	static void LeftMouseButtonUp();
	static void RightMouseButtonDown();
	static void RightMouseButtonUp();
};