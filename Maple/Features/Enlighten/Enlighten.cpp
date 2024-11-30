#include "Enlighten.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Vanilla.h"

#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../SDK/Osu/GameField.h"
#include "../../Configuration/ConfigManager.h"
#include "../../SDK/Player/Player.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Graphics/SpriteManager.h"
#include "../../SDK/Graphics/TextureShader2D.h"
#include "../../SDK/Input/InputManager.h"
#include "../../SDK/Mods/ModManager.h"
#include "../../SDK/Player/Ruleset.h"

void Enlighten::initializeOverlay()
{
	hitObjectManagerLoaded = false;
	hitObjectManagerReady = false;

	if (Enabled && Player::GetPlayMode() == PlayModes::Osu)
	{
		if (!hitObjectManager)
		{
			hitObjectManager = Ruleset::CreateHitObjectManager(Ruleset::GetInstance());

			Vanilla::AddRelocation(std::ref(hitObjectManager));
		}

		// todo: let hitobjectmanager hooks handle mods and preempt
		HitObjectManager::SetBeatmap(hitObjectManager, Player::GetBeatmapInstance(), Mods::None);
	}
}

void Enlighten::initializePreemptiveDots()
{
	preemtiveDotRadius = HitObjectManager::GetSpriteDisplaySize() / 15.f;
	preemptiveDots.clear();

	const Vector2 clientPosition = GameBase::GetClientPosition();
	const ImVec2 positionOffset = ImVec2(clientPosition.X, clientPosition.Y);
	
	preEmpt = HitObjectManager::GetPreEmpt(true);

	for (int i = 0; i < HitObjectManager::GetHitObjectsCount(); i++)
	{
		const HitObject previousHitObject = i == 0 ? HitObject() : HitObjectManager::GetHitObject(i - 1);
		const HitObject hitObject = HitObjectManager::GetHitObject(i);

		const Vector2 displayPos = GameField::FieldToDisplay(hitObject.Position);
		if (previousHitObject.IsNull || hitObject.StartTime - previousHitObject.EndTime > preEmpt)
			preemptiveDots.emplace_back(ImVec2(displayPos.X, displayPos.Y) + positionOffset, hitObject.StartTime - preEmpt);
	}
}

void Enlighten::Initialize()
{
	initializeOverlay();
	initializePreemptiveDots();
}

void Enlighten::RenderOverlayBackground() // just in case we won't be able to render beatmap background for some reason
{
	if (Enabled && Player::GetIsLoaded() && !Player::GetIsReplayMode() && Player::GetPlayMode() == PlayModes::Osu && !AudioEngine::GetIsPaused())
	{
		const Vector2 clientPosition = GameBase::GetClientPosition();
		const Vector2 clientSize = GameBase::GetClientSize();

		const ImVec2 positionOffset = ImVec2(clientPosition.X, clientPosition.Y);
		const ImVec2 windowSize = ImVec2(clientSize.X, clientSize.Y);

		ImGui::GetBackgroundDrawList()->AddRectFilled(positionOffset, positionOffset + windowSize, ImColor(0, 0, 0, 255));
	}
}

void Enlighten::RenderOverlay()
{
	if (Enabled && Player::GetIsLoaded() && !Player::GetIsReplayMode() && !AudioEngine::GetIsPaused())
	{
		if (!hitObjectManagerLoaded)
		{
			hitObjectManagerLoaded = true;
			hitObjectManagerReady = HitObjectManager::Load(hitObjectManager, false, false);
		}

		isOverlayCurrentlyRendering = true;

		uintptr_t originalHitObjectManager = Player::GetHitObjectManager();
		if (!hitObjectManager || !originalHitObjectManager || !hitObjectManagerReady)
		{
			isOverlayCurrentlyRendering = false;

			return;
		}
			
		Player::SetHitObjectManager(hitObjectManager);

		if (isGamePressScheduled)
			Player::DoOnGamePress();

		Player::UpdateActive();

		HitObjectManager::UpdateBasic(hitObjectManager);

		if (InputManager::GetScorableFrame())
			Ruleset::UpdateScoring();

		HitObjectManager::UpdateHitObjects(hitObjectManager);

		TextureShader2D::Begin();
		// todo: draw TransitionManager.spriteManagerBack and TransitionManager.spriteManagerBackWide
		Player::Draw();
		// todo: maybe draw GameBase.FpsDisplay, GameBase.Volume and GameBase.cursorTrailRenderer
		SpriteManager::Draw(GameBase::GetSpriteManagerCursor());
		TextureShader2D::End();

		Player::SetHitObjectManager(originalHitObjectManager);
		Player::UpdateActive();

		isOverlayCurrentlyRendering = false;
	}
}

void Enlighten::RenderPreemptiveDots()
{
	if (ConfigManager::CurrentConfig.Visuals.ARChanger.Enabled && ConfigManager::CurrentConfig.Visuals.ARChanger.DrawPreemptiveDot && Player::GetIsLoaded() && !Player::GetIsReplayMode() && (Player::GetPlayMode() == PlayModes::Osu || Player::GetPlayMode() == PlayModes::CatchTheBeat))
	{
		for (unsigned int i = 0; i < preemptiveDots.size(); i++)
		{
			const int time = std::get<1>(preemptiveDots[i]);
			if (AudioEngine::GetTime() >= time && AudioEngine::GetTime() < time + preEmpt)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(std::get<0>(preemptiveDots[i]), preemtiveDotRadius, ImColor(ConfigManager::CurrentConfig.Visuals.ARChanger.PreemptiveDotColour), 32);
		}
	}
}

uintptr_t Enlighten::GetHitObjectManager()
{
	return hitObjectManager;
}

bool Enlighten::GetIsOverlayCurrentlyRendering()
{
	return isOverlayCurrentlyRendering;
}

void Enlighten::ScheduleGamePress()
{
	isGamePressScheduled = true;
}

void Enlighten::DrawHitObjectManager()
{
	SpriteManager::Draw(HitObjectManager::GetSpriteManagerInstance(hitObjectManager));
}
