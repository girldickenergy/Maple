#include "Ruleset.h"

#include "Player.h"
#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Config/Config.h"

void __declspec(naked) Ruleset::loadFlashlightHook(uintptr_t instance)
{
	__asm
	{
		pushad
		pushfd
		cmp[Config::Visuals::Removers::FlashlightRemoverEnabled], 0x0
		je orig
		call GameBase::GetMode
		cmp eax, 0x2
		jne orig
		call Player::GetIsReplayMode
		cmp al, 0x0
		jne orig
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oLoadFlashlight
	}
}

void __declspec(naked) Ruleset::loadManiaFlashlightHook(uintptr_t instance)
{
	__asm
	{
		pushad
		pushfd
		cmp[Config::Visuals::Removers::FlashlightRemoverEnabled], 0x0
		je orig
		call GameBase::GetMode
		cmp eax, 0x2
		jne orig
		call Player::GetIsReplayMode
		cmp al, 0x0
		jne orig
		popfd
		popad
		ret
		orig:
		popfd
		popad
		jmp oLoadManiaFlashlight
	}
}

int __declspec(naked) Ruleset::hasHiddenSpritesHook(uintptr_t instance)
{
	__asm
	{
		push ecx
		push edx
		push ebx
		push esp
		push ebp
		push esi
		push edi
		pushfd

		cmp[Config::Visuals::Removers::HiddenRemoverEnabled], 0x0
		je orig
		call GameBase::GetMode
		cmp eax, 0x2
		jne orig
		call Player::GetIsReplayMode
		cmp al, 0x0
		jne orig

		popfd
		pop edi
		pop esi
		pop ebp
		add esp, 0x4
		pop ebx
		pop edx
		pop ecx

		mov eax, 0x0
		ret

		orig:
		popfd
		pop edi
		pop esi
		pop ebp
		add esp, 0x4
		pop ebx
		pop edx
		pop ecx

		jmp oHasHiddenSprites
	}
}

void Ruleset::Initialize()
{
	Memory::AddObject("Ruleset::IncreaseScoreHit", "55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 89 55 F0 83 7D F0 00");
	Memory::AddPatch("Ruleset::IncreaseScoreHit_HackCheck", "Ruleset::IncreaseScoreHit", "80 78 7C 00 0F 84", 0x1D36, 0x5, { 0x8D });

	Memory::AddObject("Ruleset::LoadFlashlight", "55 8B EC 57 56 53 83 EC 30 8B F1 8D 7D C8 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D D8 8B 45 D8 83 78 5C 00");
	Memory::AddHook("Ruleset::LoadFlashlight", "Ruleset::LoadFlashlight", reinterpret_cast<uintptr_t>(loadFlashlightHook), reinterpret_cast<uintptr_t*>(&oLoadFlashlight));

	Memory::AddObject("RulesetMania::LoadFlashlight", "55 8B EC 57 56 53 83 EC 30 8B F1 8D 7D C4 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D D4 8B 45 D4 83 78 5C 00");
	Memory::AddHook("RulesetMania::LoadFlashlight", "RulesetMania::LoadFlashlight", reinterpret_cast<uintptr_t>(loadManiaFlashlightHook), reinterpret_cast<uintptr_t*>(&oLoadManiaFlashlight));

	Memory::AddObject("RulesetMania::StageMania::GetHasHiddenSprites", "55 8B EC 56 A1 ?? ?? ?? ?? 85 C0 74 24 8B 50 1C 8B 4A 04 8B 72 08 FF 72 0C 8B D6");
	Memory::AddHook("RulesetMania::StageMania::GetHasHiddenSprites", "RulesetMania::StageMania::GetHasHiddenSprites", reinterpret_cast<uintptr_t>(hasHiddenSpritesHook), reinterpret_cast<uintptr_t*>(&oHasHiddenSprites));
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
