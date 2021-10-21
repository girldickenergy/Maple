#include "VisualsSpoofers.h"

#include "../../Config/Config.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Player/Player.h"
#include "../../Sdk/Player/Ruleset.h"

void VisualsSpoofers::spoofVisuals()
{
	if (GameBase::Mode() != OsuModes::Play || Player::IsReplayMode())
		return;

	originalPreEmpt = HitObjectManager::GetPreEmpt();
	originalPreEmptSliderComplete = HitObjectManager::GetPreEmptSliderComplete();
	
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
	if (GameBase::Mode() != OsuModes::Play || Player::IsReplayMode())
		return;

	if (Config::Visuals::ARChangerEnabled)
	{
		const int preEmpt = static_cast<int>(HitObjectManager::MapDifficultyRange(static_cast<double>(Config::Visuals::AR), 1800., 1200., 450., Config::Visuals::ARChangerAdjustToMods));

		HitObjectManager::SetPreEmpt(preEmpt);
		HitObjectManager::SetPreEmptSliderComplete(preEmpt * 2 / 3);
	}
}

void VisualsSpoofers::restorePreEmpt()
{
	if (GameBase::Mode() != OsuModes::Play || Player::IsReplayMode())
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
__declspec(naked) void VisualsSpoofers::ApplyStackingHook(void* instance)
{
	__asm
	{
		pushad
		pushfd
		call restorePreEmpt
		popfd
		popad
		jmp oApplyStacking
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

void VisualsSpoofers::FlashlightRemoverThread()
{
	while (true)
	{
		if (Player::IsLoaded() && !Player::IsReplayMode())
		{
			const float targetAlpha = roundf(Config::Visuals::FlashlightDisabled ? 0.f : 1.f);
			if (roundf(Ruleset::GetFlashlightAlpha()) != targetAlpha)
				Ruleset::SetFlashlightAlpha(targetAlpha);
		}

		Sleep(100);
	}
}
