#include "TextureShader2D.h"

#include "xorstr.hpp"

#include "../Memory.h"

void TextureShader2D::Initialize()
{
	Memory::AddObject(xorstr_("TextureShader2D::Instance"), xorstr_("74 06 0F B6 40 0C EB 02 33 C0 85 C0 74 06 FF 15 ?? ?? ?? ?? 8B 0D"), 0x16, 0x1);
	Memory::AddObject(xorstr_("TextureShader2D::Begin"), xorstr_("55 8B EC 56 8B F1 80 7E 19 00"));
	Memory::AddObject(xorstr_("TextureShader2D::End"), xorstr_("56 8B F1 80 7E 19 00 75 02"));
}

uintptr_t TextureShader2D::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects[xorstr_("TextureShader2D::Instance")];

	return instanceAddress ? *reinterpret_cast<uintptr_t*>(instanceAddress) : 0u;
}

void TextureShader2D::Begin()
{
	const uintptr_t beginFunctionAddress = Memory::Objects[xorstr_("TextureShader2D::Begin")];
	if (const uintptr_t instance = GetInstance(); instance && beginFunctionAddress)
		reinterpret_cast<fnBeginEnd>(beginFunctionAddress)(instance);
}

void TextureShader2D::End()
{
	const uintptr_t endFunctionAddress = Memory::Objects[xorstr_("TextureShader2D::End")];
	if (const uintptr_t instance = GetInstance(); instance && endFunctionAddress)
		reinterpret_cast<fnBeginEnd>(endFunctionAddress)(instance);
}
