#include "Enlighten.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../SDK/Osu/GameField.h"
#include "../../Config/Config.h"
#include "../../SDK/Player/Player.h"
#include "../../SDK/Audio/AudioEngine.h"

void Enlighten::initializePreemptiveDots()
{
	preemtiveDotRadius = HitObjectManager::GetSpriteDisplaySize() / 15.f;
	preemptiveDots.clear();

	const Vector2 clientPosition = GameBase::GetClientPosition();
	const ImVec2 positionOffset = ImVec2(clientPosition.X, clientPosition.Y);
	
	preEmpt = HitObjectManager::GetPreEmpt();

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
	initializePreemptiveDots();
}

void Enlighten::Render()
{
	if (Config::Visuals::ARChanger::Enabled && Config::Visuals::ARChanger::DrawPreemptiveDot && Player::GetIsLoaded() && !Player::GetIsReplayMode() && (Player::GetPlayMode() == PlayModes::Osu || Player::GetPlayMode() == PlayModes::CatchTheBeat))
	{
		for (unsigned int i = 0; i < preemptiveDots.size(); i++)
		{
			const int time = std::get<1>(preemptiveDots[i]);
			if (AudioEngine::GetTime() >= time && AudioEngine::GetTime() < time + preEmpt)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(std::get<0>(preemptiveDots[i]), preemtiveDotRadius, ImColor(Config::Visuals::ARChanger::PreemptiveDotColour), 32);
		}
	}
}
