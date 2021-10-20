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
		originalPreEmpt = HitObjectManager::GetPreEmpt();
		originalPreEmptSliderComplete = HitObjectManager::GetPreEmptSliderComplete();
		
		double difficulty = round(static_cast<double>(Config::Visuals::AR));

		if (Config::Visuals::ARChangerAdjustToMods)
			difficulty = ModManager::IsModEnabled(Mods::Easy) ? max(0.0, difficulty / 2.0) : ModManager::IsModEnabled(Mods::HardRock) ? min(10.0, difficulty * 1.4) : difficulty;

		if (difficulty > 5.)
			difficulty = 1200. + (450. - 1200.) * (difficulty - 5.) / 5.;
		else if (difficulty < 5.)
			difficulty = 1200. - (1200. - 1800.) * (5. - difficulty) / 5.;
		else
			difficulty = 1200.;

		HitObjectManager::SetPreEmpt(static_cast<int>(difficulty));
		HitObjectManager::SetPreEmptSliderComplete(static_cast<int>(difficulty * 2. / 3.));
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
