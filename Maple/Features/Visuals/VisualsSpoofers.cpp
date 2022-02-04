#include "VisualsSpoofers.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "../../Config/Config.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/Player.h"
#include "../Timewarp/Timewarp.h"
#include "../../Sdk/Osu/GameField.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Osu/WindowManager.h"

void VisualsSpoofers::spoofVisuals()
{
	if (GameBase::Mode() != OsuModes::Play || Player::IsReplayMode())
		return;
	
	spoofPreEmpt();

	if (Config::Visuals::CSChangerEnabled)
	{
		float spriteDisplaySize = GameField::GetWidth() / 8.f * (1.f - 0.7f * ((Config::Visuals::CS - 5.f) / 5.f));
		float hitObjectRadius = spriteDisplaySize / 2.f / GameField::GetRatio() * 1.00041f;
		float spriteRatio = spriteDisplaySize / 128.f;

		HitObjectManager::SetSpriteDisplaySize(spriteDisplaySize);
		HitObjectManager::SetHitObjectRadius(hitObjectRadius);
		HitObjectManager::SetSpriteRatio(spriteRatio);
		HitObjectManager::SetGamefieldSpriteRatio(spriteRatio);
		HitObjectManager::SetStackOffset(hitObjectRadius / 10.f);
	}
	
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
		const double rateMultiplier = Config::Visuals::ARChangerAdjustToRate ? ((Config::Timewarp::Enabled ? Timewarp::GetRate() : ModManager::ModPlaybackRate()) / 100.) : 1.;
		const int preEmpt = static_cast<int>(HitObjectManager::MapDifficultyRange(static_cast<double>(Config::Visuals::AR), 1800., 1200., 450., Config::Visuals::ARChangerAdjustToMods) * rateMultiplier);

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

void VisualsSpoofers::LoadPreemptiveDots()
{
	preemtiveDotRadius = HitObjectManager::GetSpriteDisplaySize() / 15.f;
	preemptiveDots.clear();

	Vector2 viewportPosition = WindowManager::ViewportPosition();
	ImVec2 positionOffset = ImVec2(viewportPosition.X, viewportPosition.Y);

	for (int i = 0; i < HitObjectManager::GetHitObjectsCount(); i++)
	{
		HitObject previousHitObject = i == 0 ? HitObject() : HitObjectManager::GetHitObject(i - 1);
		HitObject hitObject = HitObjectManager::GetHitObject(i);

		Vector2 displayPos = GameField::FieldToDisplay(hitObject.Position);
		if (previousHitObject.IsNull || hitObject.StartTime - previousHitObject.EndTime > originalPreEmpt)
			preemptiveDots.emplace_back(ImVec2(displayPos.X, displayPos.Y) + positionOffset, hitObject.StartTime - originalPreEmpt);
	}
}

void VisualsSpoofers::DrawPreemptiveDots()
{
	if (Config::Visuals::ARChangerEnabled && Config::Visuals::ARChangerDrawPreemptiveDot && Player::IsLoaded() && !Player::IsReplayMode())
	{
		for (int i = 0; i < preemptiveDots.size(); i++)
		{
			int time = std::get<1>(preemptiveDots[i]);
			if (AudioEngine::Time() >= time && AudioEngine::Time() < time + originalPreEmpt)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(std::get<0>(preemptiveDots[i]), preemtiveDotRadius, ImColor(Config::Visuals::ARChangerPreemptiveDotColour), 32);
		}
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
