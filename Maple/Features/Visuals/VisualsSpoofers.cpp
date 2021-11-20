#include "VisualsSpoofers.h"

#include "../../Config/Config.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Player/Player.h"

void VisualsSpoofers::spoofVisuals()
{
	if (GameBase::Mode() != OsuModes::Play || Player::IsReplayMode())
		return;
	
	spoofPreEmpt();
	
	if (Config::Visuals::HiddenDisabled)
	{
		Mods mods = HitObjectManager::GetActiveMods();

		if (ModManager::IsModEnabled(Mods::Hidden))
			mods = (mods & ~Mods::Hidden);

		HitObjectManager::SetActiveMods(mods);
	}
}

void VisualsSpoofers::spoofPreEmpt()
{
	if (GameBase::Mode() != OsuModes::Play || (Player::PlayMode() != PlayModes::Osu && Player::PlayMode() != PlayModes::Catch) || Player::IsReplayMode())
		return;

	originalPreEmpt = HitObjectManager::GetPreEmpt();
	originalPreEmptSliderComplete = HitObjectManager::GetPreEmptSliderComplete();

	if (Config::Visuals::ARChangerEnabled)
	{
		const int preEmpt = static_cast<int>(HitObjectManager::MapDifficultyRange(static_cast<double>(Config::Visuals::AR), 1800., 1200., 450., Config::Visuals::ARChangerAdjustToMods));

		HitObjectManager::SetPreEmpt(preEmpt);
		HitObjectManager::SetPreEmptSliderComplete(preEmpt * 2 / 3);
	}
}

void VisualsSpoofers::restorePreEmpt()
{
	if (GameBase::Mode() != OsuModes::Play || (Player::PlayMode() != PlayModes::Osu && Player::PlayMode() != PlayModes::Catch) || Player::IsReplayMode())
		return;

	if (Config::Visuals::ARChangerEnabled)
	{
		HitObjectManager::SetPreEmpt(originalPreEmpt);
		HitObjectManager::SetPreEmptSliderComplete(originalPreEmptSliderComplete);
	}
}

__declspec(naked) void VisualsSpoofers::ParseHook(void* instance, int sectionsToParse, BOOL updateChecksum, BOOL applyParsingLimits)
{
	__asm
	{
		pushad
		pushfd
		call spoofVisuals
		popfd
		popad
		jmp oParse
	}
}

//restore original preempts before stacking to fix stacking issue
__declspec(naked) void VisualsSpoofers::UpdateStackingHook(void* instance, int startIndex, int endIndex)
{
	__asm
	{
		pushad
		pushfd
		call restorePreEmpt
		popfd
		popad
		jmp oUpdateStacking
	}
}

__declspec(naked) void VisualsSpoofers::ApplyOldStackingHook(void* instance)
{
	__asm
	{
		pushad
		pushfd
		call restorePreEmpt
		popfd
		popad
		jmp oApplyOldStacking
	}
}

//aaaand spoof again because peppy is an idiot
__declspec(naked) void VisualsSpoofers::AddFollowPointsHook(void* instance, int startIndex, int endIndex)
{
	__asm
	{
		pushad
		pushfd
		call spoofPreEmpt
		popfd
		popad
		jmp oAddFollowPoints
	}
}

void __fastcall VisualsSpoofers::LoadFlashlightHook(void* instance)
{
	if (Config::Visuals::FlashlightDisabled && GameBase::Mode() == OsuModes::Play && !Player::IsReplayMode())
		return;

	oLoadFlashlight(instance);
}

void __fastcall VisualsSpoofers::LoadFlashlightManiaHook(void* instance)
{
	if (Config::Visuals::FlashlightDisabled && GameBase::Mode() == OsuModes::Play && !Player::IsReplayMode())
		return;

	oLoadFlashlightMania(instance);
}

void __fastcall VisualsSpoofers::UpdateFlashlightHook(void* instance)
{
	if (Config::Visuals::FlashlightDisabled && GameBase::Mode() == OsuModes::Play && !Player::IsReplayMode())
		return;

	oUpdateFlashlight(instance);
}

BOOL __fastcall VisualsSpoofers::HasHiddenSpritesHook(void* instance)
{
	if (Config::Visuals::HiddenDisabled && GameBase::Mode() == OsuModes::Play && !Player::IsReplayMode())
		return FALSE;

	return oHasHiddenSprites(instance);
}
