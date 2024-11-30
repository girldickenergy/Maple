#include "Ruleset.h"

#include "VirtualizerSDK.h"

#include "Player.h"
#include "xorstr.hpp"
#include "../Memory.h"
#include "../Osu/GameBase.h"
#include "../../Configuration/ConfigManager.h"
#include "../../Communication/Communication.h"
#include "../../Features/Enlighten/Enlighten.h"

void __fastcall Ruleset::loadFlashlightHook(uintptr_t instance)
{
	if (!ConfigManager::CurrentConfig.Visuals.Removers.FlashlightRemoverEnabled || GameBase::GetMode() != OsuModes::Play || Player::GetIsReplayMode())
		[[clang::musttail]] return oLoadFlashlight(instance);
}

void __fastcall Ruleset::loadManiaFlashlightHook(uintptr_t instance)
{
	if (!ConfigManager::CurrentConfig.Visuals.Removers.FlashlightRemoverEnabled || GameBase::GetMode() != OsuModes::Play || Player::GetIsReplayMode())
		[[clang::musttail]] return oLoadManiaFlashlight(instance);
}

int __fastcall Ruleset::hasHiddenSpritesHook(uintptr_t instance)
{
	if (!ConfigManager::CurrentConfig.Visuals.Removers.HiddenRemoverEnabled || GameBase::GetMode() != OsuModes::Play || Player::GetIsReplayMode())
		[[clang::musttail]] return oHasHiddenSprites(instance);

	return FALSE;
}

void __fastcall Ruleset::increaseScoreHitHook(uintptr_t instance, int value, uintptr_t hitObject)
{
	if (Enlighten::GetIsOverlayCurrentlyRendering())
		return;

	[[clang::musttail]] return oIncreaseScoreHit(instance, value, hitObject);
}

void Ruleset::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("Ruleset::IncreaseScoreHit"), xorstr_("55 8B EC 57 56 53 81 EC ?? ?? ?? ?? 8B F1 8D BD ?? ?? ?? ?? B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 8D ?? ?? ?? ?? 89 55 F0 83 7D F0 00 75 0A"));
	Memory::AddPatch(xorstr_("Ruleset::IncreaseScoreHit_HackCheck"), xorstr_("Ruleset::IncreaseScoreHit"), xorstr_("80 78 7C 00 0F 84"), 0x1D36, 0x5, { 0x8D });

	Memory::AddObject(xorstr_("Ruleset::LoadFlashlight"), xorstr_("55 8B EC 57 56 53 83 EC 30 8B F1 8D 7D C8 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D D8 8B 45 D8 83 78 5C 00"));
	Memory::AddHook(xorstr_("Ruleset::LoadFlashlight"), xorstr_("Ruleset::LoadFlashlight"), reinterpret_cast<uintptr_t>(loadFlashlightHook), reinterpret_cast<uintptr_t*>(&oLoadFlashlight));

	Memory::AddObject(xorstr_("RulesetMania::LoadFlashlight"), xorstr_("55 8B EC 57 56 53 83 EC 30 8B F1 8D 7D C4 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D D4 8B 45 D4 83 78 5C 00"));
	Memory::AddHook(xorstr_("RulesetMania::LoadFlashlight"), xorstr_("RulesetMania::LoadFlashlight"), reinterpret_cast<uintptr_t>(loadManiaFlashlightHook), reinterpret_cast<uintptr_t*>(&oLoadManiaFlashlight));

	Memory::AddObject(xorstr_("RulesetMania::StageMania::GetHasHiddenSprites"), xorstr_("55 8B EC 56 A1 ?? ?? ?? ?? 85 C0 74 24 8B 50 1C 8B 4A 04 8B 72 08 FF 72 0C 8B D6"));
	Memory::AddHook(xorstr_("RulesetMania::StageMania::GetHasHiddenSprites"), xorstr_("RulesetMania::StageMania::GetHasHiddenSprites"), reinterpret_cast<uintptr_t>(hasHiddenSpritesHook), reinterpret_cast<uintptr_t*>(&oHasHiddenSprites));

	Memory::AddObject(xorstr_("RulesetOsu::CreateHitObjectManager"), xorstr_("55 8B EC 56 E8 ?? ?? ?? ?? 85 C0 74 18 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0"));
	Memory::AddObject(xorstr_("Ruleset::UpdateScoring"), xorstr_("55 8B EC 57 56 50 8B F9 8B 47 44"));
	Memory::AddObject(xorstr_("RulesetOsu::IncreaseScoreHit"), xorstr_("55 8B EC 57 56 53 8B F9 8B F2 8B 5D 08 8B CB"));
	Memory::AddHook(xorstr_("RulesetOsu::IncreaseScoreHit"), xorstr_("RulesetOsu::IncreaseScoreHit"), reinterpret_cast<uintptr_t>(increaseScoreHitHook), reinterpret_cast<uintptr_t*>(&oIncreaseScoreHit));

	VIRTUALIZER_FISH_RED_END
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

uintptr_t Ruleset::CreateHitObjectManager(uintptr_t instance)
{
	const uintptr_t createHitObjectManagerFunctionAddress = Memory::Objects[xorstr_("RulesetOsu::CreateHitObjectManager")];

	return createHitObjectManagerFunctionAddress ? reinterpret_cast<fnCreateHitObjectManager>(createHitObjectManagerFunctionAddress)(instance) : 0u;
}

void Ruleset::UpdateScoring()
{
	const uintptr_t instance = GetInstance();
	const uintptr_t updateScoringFunctionAddress = Memory::Objects[xorstr_("Ruleset::UpdateScoring")];

	if (instance)
		reinterpret_cast<fnUpdateScoring>(updateScoringFunctionAddress)(instance);
}
