#include "HitObjectManager.h"

#include "Player.h"
#include "../Helpers/Obfuscated.h"

void HitObjectManager::Initialize()
{

}

uintptr_t HitObjectManager::GetInstance()
{
	const uintptr_t playerInstance = Player::GetInstance();

	return playerInstance ? *reinterpret_cast<uintptr_t*>(playerInstance + HITOBJECTMANAGER_INSTANCE_OFFSET) : 0u;
}

int HitObjectManager::GetPreEmpt()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPT_OFFSET) : 0;
}

void HitObjectManager::SetPreEmpt(int value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPT_OFFSET) = value;
}

int HitObjectManager::GetPreEmptSliderComplete()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPTSLIDERCOMPLETE_OFFSET) : 0;
}

void HitObjectManager::SetPreEmptSliderComplete(int value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_PREEMPTSLIDERCOMPLETE_OFFSET) = value;
}

Mods HitObjectManager::GetActiveMods()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? static_cast<Mods>(Obfuscated::GetInt(*reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_ACTIVEMODS_OFFSET))) : Mods::None;
}

void HitObjectManager::SetActiveMods(Mods value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		Obfuscated::SetInt(*reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_ACTIVEMODS_OFFSET), static_cast<int>(value));
}

int HitObjectManager::GetHitWindow50()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW50_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow100()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW100_OFFSET) : 0;
}

int HitObjectManager::GetHitWindow300()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITWINDOW300_OFFSET) : 0;
}

float HitObjectManager::GetSpriteDisplaySize()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEDISPLAYSIZE_OFFSET) : 0.f;
}

void HitObjectManager::SetSpriteDisplaySize(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEDISPLAYSIZE_OFFSET) = value;
}

float HitObjectManager::GetHitObjectRadius()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET) : 0.f;
}

void HitObjectManager::SetHitObjectRadius(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTRADIUS_OFFSET) = value;
}

float HitObjectManager::GetSpriteRatio()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITERATIO_OFFSET) : 0.f;
}

void HitObjectManager::GetSpriteRatio(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITERATIO_OFFSET) = value;
}

uintptr_t HitObjectManager::GetSpriteManagerInstance()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<uintptr_t*>(hitObjectManagerInstance + HITOBJECTMANAGER_SPRITEMANAGER_OFFSET) : 0u;
}

float HitObjectManager::GetGamefieldSpriteRatio()
{
	return 0.0f;
}

void HitObjectManager::SetGamefieldSpriteRatio(float value)
{
}

float HitObjectManager::GetStackOffset()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_STACKOFFSET_OFFSET) : 0.f;
}

void HitObjectManager::SetStackOffset(float value)
{
	if (const uintptr_t hitObjectManagerInstance = GetInstance())
		*reinterpret_cast<float*>(hitObjectManagerInstance + HITOBJECTMANAGER_STACKOFFSET_OFFSET) = value;
}

int HitObjectManager::GetCurrentHitObjectIndex()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_CURRENTHITOBJECTINDEX_OFFSET) : 0.f;
}

int HitObjectManager::GetHitObjectsCount()
{
	const uintptr_t hitObjectManagerInstance = GetInstance();

	return hitObjectManagerInstance ? *reinterpret_cast<int*>(hitObjectManagerInstance + HITOBJECTMANAGER_HITOBJECTSCOUNT_OFFSET) : 0.f;
}
