#pragma once
#include <cstdint>

class TextureShader2D
{
	typedef void(__fastcall* fnBeginEnd)(uintptr_t instance);
public:
	static void Initialize();

	static uintptr_t GetInstance();
	static void Begin();
	static void End();
};