#include "SpriteManager.h"

#include "xorstr.hpp"

#include "../Memory.h"

void SpriteManager::Initialize()
{
	Memory::AddObject(xorstr_("SpriteManager::Draw"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 85"));
}

float SpriteManager::GetGamefieldSpriteRatio(uintptr_t instance)
{
	return instance ? *reinterpret_cast<float*>(instance + GAMEFIELDSPRITERATIO_OFFSET) : 0.f;
}

void SpriteManager::SetGamefieldSpriteRatio(uintptr_t instance, float value)
{
	if (instance)
		*reinterpret_cast<float*>(instance + GAMEFIELDSPRITERATIO_OFFSET) = value;
}

bool SpriteManager::Draw(uintptr_t instance)
{
	const uintptr_t drawFunctionAddress = Memory::Objects[xorstr_("SpriteManager::Draw")];
	if (!drawFunctionAddress)
		return false;

	return reinterpret_cast<fnDraw>(drawFunctionAddress)(instance);
}
