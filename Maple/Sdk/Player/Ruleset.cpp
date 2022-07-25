#include "Ruleset.h"

#include "Player.h"
#include "../Memory.h"

void Ruleset::Initialize()
{
	Memory::AddObject("Ruleset::IncreaseScoreHit", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 89 55 F0 83 7D F0 00");
	Memory::AddPatch("Ruleset::IncreaseScoreHit_HackCheck", "Ruleset::IncreaseScoreHit", "80 78 7C 00 0F 84", 0x1D36, 0x5, { 0x8D });
}

uintptr_t Ruleset::GetInstance()
{
	const uintptr_t playerInstance = Player::GetInstance();
	if (!playerInstance)
		return 0u;

	return *reinterpret_cast<uintptr_t*>(playerInstance + INSTANCE_OFFSET);
}

float Ruleset::GetCatcherSpeed()
{
	const uintptr_t instance = GetInstance();
	if (!instance)
		return 1.f;

	return *reinterpret_cast<float*>(instance + BASEMOVEMENTSPEED_OFFSET);
}

void Ruleset::SetCatcherSpeed(float value)
{
	if (const uintptr_t instance = GetInstance())
		*reinterpret_cast<float*>(instance + BASEMOVEMENTSPEED_OFFSET) = value;
}
