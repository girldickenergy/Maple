#pragma once

class GLControl
{
	typedef bool(__fastcall* fnGetUsesAngle)();
public:
	static void Initialize();

	static bool GetUsesAngle();
};