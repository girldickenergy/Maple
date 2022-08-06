#include "SpriteManager.h"

float SpriteManager::GetGamefieldSpriteRatio(uintptr_t instance)
{
	return instance ? *reinterpret_cast<float*>(instance + GAMEFIELDSPRITERATIO_OFFSET) : 0.f;
}

void SpriteManager::SetGamefieldSpriteRatio(uintptr_t instance, float value)
{
	if (instance)
		*reinterpret_cast<float*>(instance + GAMEFIELDSPRITERATIO_OFFSET) = value;
}
