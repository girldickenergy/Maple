#include "TaikoMania.h"

#include <sstream>
#include <iomanip>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "../../Configuration/ConfigManager.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../SDK/Player/Player.h"
#include "../../UI/StyleProvider.h"
#include "../../SDK/Scoring/Score.h"
#include "../../SDK/Memory.h"

void TaikoMania::Initialize()
{
	if (Player::GetPlayMode() != PlayModes::Taiko)
		return;

	stages.clear();

	auto primaryDonStage = TaikoManiaStage(TaikoManiaObjectType::Don);
	auto secondaryDonStage = TaikoManiaStage(TaikoManiaObjectType::Don);
	auto primaryKatsuStage = TaikoManiaStage(TaikoManiaObjectType::Katsu);
	auto secondaryKatsuStage = TaikoManiaStage(TaikoManiaObjectType::Katsu);

	int lastDonTime = INT_MIN;
	int lastKatsuTime = INT_MIN;
	bool isLastDonStagePrimary = false;
	bool isLastKatsuStagePrimary = false;
	for (int i = 0; i < HitObjectManager::GetHitObjectsCount(); i++)
	{
		auto hitObject = HitObjectManager::GetHitObject(i);

		if (hitObject.IsType(HitObjectType::Normal))
		{
			auto type = hitObject.HasHitSound(HitSoundType::Whistle) || hitObject.HasHitSound(HitSoundType::Clap) ? TaikoManiaObjectType::Katsu : TaikoManiaObjectType::Don;
			auto isLarge = hitObject.HasHitSound(HitSoundType::Finish);

			bool isDonStagePrimary = true;
			bool isKatsuStagePrimary = true;

			if ((isLarge || type == TaikoManiaObjectType::Don) && lastDonTime != INT_MIN && 60000 / std::clamp(hitObject.StartTime - lastDonTime, 1, 60000) / 4 >= ConfigManager::CurrentConfig.Visuals.TaikoMania.AlternateBPM)
				isDonStagePrimary = !isLastDonStagePrimary;
			else if ((isLarge || type == TaikoManiaObjectType::Katsu) && lastKatsuTime != INT_MIN && 60000 / std::clamp(hitObject.StartTime - lastKatsuTime, 1, 60000) / 4 >= ConfigManager::CurrentConfig.Visuals.TaikoMania.AlternateBPM)
				isKatsuStagePrimary = !isLastKatsuStagePrimary;

			if (type == TaikoManiaObjectType::Don)
			{
				if (isLarge)
				{
					primaryDonStage.AddObject(hitObject.StartTime);
					secondaryDonStage.AddObject(hitObject.StartTime);
				}
				else if (isDonStagePrimary)
					primaryDonStage.AddObject(hitObject.StartTime);
				else
					secondaryDonStage.AddObject(hitObject.StartTime);

				lastDonTime = hitObject.StartTime;
				isLastDonStagePrimary = isDonStagePrimary;
			}
			else
			{
				if (isLarge)
				{
					primaryKatsuStage.AddObject(hitObject.StartTime);
					secondaryKatsuStage.AddObject(hitObject.StartTime);
				}
				if (isKatsuStagePrimary)
					primaryKatsuStage.AddObject(hitObject.StartTime);
				else
					secondaryKatsuStage.AddObject(hitObject.StartTime);

				lastKatsuTime = hitObject.StartTime;
				isLastKatsuStagePrimary = isKatsuStagePrimary;
			}
		}

		duration = hitObject.EndTime;
	}

	stages = { primaryKatsuStage, secondaryKatsuStage, primaryDonStage, secondaryDonStage };
}

void TaikoMania::Render()
{
	if (ConfigManager::CurrentConfig.Visuals.TaikoMania.Enabled && Player::GetIsLoaded() && !Player::GetIsReplayMode() && Player::GetPlayMode() == PlayModes::Taiko && !AudioEngine::GetIsPaused())
	{
		const Vector2 clientPosition = GameBase::GetClientPosition();
		const Vector2 clientSize = GameBase::GetClientSize();

		const ImVec2 positionOffset = ImVec2(clientPosition.X, clientPosition.Y);
		const ImVec2 windowSize = ImVec2(clientSize.X, clientSize.Y);

		const ImVec2 playfieldSize = ImVec2(clientSize.X / 4, clientSize.Y);
		const ImVec2 playfieldStart = positionOffset + ImVec2(windowSize.x / 2 - playfieldSize.x / 2, 0);
		const ImVec2 playfieldEnd = positionOffset + ImVec2(windowSize.x / 2 + playfieldSize.x / 2, playfieldSize.y);

		ImGui::GetBackgroundDrawList()->AddRectFilled(positionOffset, positionOffset + windowSize, ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.BackgroundColour));

		ImGui::GetBackgroundDrawList()->AddRectFilled(playfieldStart - ImVec2(5,5), playfieldEnd + ImVec2(5, 5), ImColor(255, 255, 255, 255));
		ImGui::GetBackgroundDrawList()->AddRectFilled(playfieldStart, playfieldEnd, ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.PlayfieldColour));

		auto scrollSpeed = ConfigManager::CurrentConfig.Visuals.TaikoMania.ScrollSpeed;
		auto isRect = ConfigManager::CurrentConfig.Visuals.TaikoMania.NoteStyle == 0;
		auto stageSpacing = ConfigManager::CurrentConfig.Visuals.TaikoMania.StageSpacing;
		auto objectRadius = (playfieldSize.x / stages.size() - stageSpacing * 2) / 2;
		auto objectSize = ImVec2(objectRadius * 2, isRect ? objectRadius : objectRadius * 2);
		auto distance = (playfieldSize.y - objectSize.y / 2 - stageSpacing) - (playfieldStart.y - objectSize.y / 2);

		if (ConfigManager::CurrentConfig.Visuals.TaikoMania.ShowStageSeparators)
		{
			for (int i = 1; i <= 3; i++)
				ImGui::GetBackgroundDrawList()->AddLine(playfieldStart + ImVec2((playfieldSize.x / stages.size()) * i, 0), playfieldStart + ImVec2((playfieldSize.x / stages.size()) * i, playfieldSize.y), ImColor(255, 255, 255, 255), 3);

			ImGui::GetBackgroundDrawList()->AddLine(playfieldEnd - ImVec2(playfieldSize.x, objectSize.y + stageSpacing * 2), playfieldEnd - ImVec2(0, objectSize.y + stageSpacing * 2), ImColor(255, 255, 255, 255), 3);
		}

		auto time = AudioEngine::GetTime();
		for (size_t i = 0; i < stages.size(); i++)
		{
			float xCenter = playfieldStart.x + stageSpacing + i * (playfieldSize.x / 4) + objectSize.x / 2;

			for (auto objectTime : stages[i].GetObjects())
			{
				if (time >= objectTime || time < objectTime - 1200 / scrollSpeed)
					continue;

				float t = (time - (objectTime - 1000.f / scrollSpeed)) / (1000.f / scrollSpeed);
				float yCenter = (playfieldStart.y - objectSize.y / 2) + distance * t;

				if (isRect)
					ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(xCenter - objectSize.x / 2, yCenter - objectSize.y / 2), ImVec2(xCenter + objectSize.x / 2, yCenter + objectSize.y / 2), stages[i].GetType() == TaikoManiaObjectType::Don ? ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.DonColour) : ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.KatsuColour), 10);
				else
					ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(xCenter, yCenter), objectRadius, stages[i].GetType() == TaikoManiaObjectType::Don ? ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.DonColour) : ImColor(ConfigManager::CurrentConfig.Visuals.TaikoMania.KatsuColour), 32);
			}
		}

		for (size_t i = 0; i < stages.size(); i++)
		{
			if (isRect)
				ImGui::GetBackgroundDrawList()->AddRect(playfieldStart + ImVec2((playfieldSize.x / stages.size()) * i + stageSpacing, playfieldSize.y - objectSize.y - stageSpacing), playfieldStart + ImVec2((playfieldSize.x / stages.size()) * i + objectSize.x + stageSpacing, playfieldSize.y - stageSpacing), ImColor(255, 255, 255, 255), 10, 0, 3);
			else
				ImGui::GetBackgroundDrawList()->AddCircle(playfieldStart + ImVec2((playfieldSize.x / stages.size()) * i + objectRadius + stageSpacing, playfieldSize.y - objectRadius - stageSpacing), objectRadius, ImColor(255, 255, 255, 255), 32, 3);
		}

		ImGui::PushFont(StyleProvider::FontEnormousBold);
		std::ostringstream scoreStream;
		scoreStream << std::setfill('0') << std::setw(8) << Score::GetScore();
		ImVec2 scoreSize = ImGui::CalcTextSize(scoreStream.str().c_str());
		ImGui::GetBackgroundDrawList()->AddText(positionOffset + ImVec2(windowSize.x - scoreSize.x - 10, 10), ImColor(255, 255, 255, 255), scoreStream.str().c_str());
		ImGui::PopFont();

		ImGui::PushFont(StyleProvider::FontVeryHugeSemiBold);
		std::string comboStr = std::to_string(Score::GetCombo()) + "x";
		ImGui::GetBackgroundDrawList()->AddText(positionOffset + ImVec2(windowSize.x - scoreSize.x - 10, scoreSize.y + 10), ImColor(255, 255, 255, 255), comboStr.c_str());

		std::ostringstream accuracyStream;
		accuracyStream << std::fixed << std::setprecision(2) << Score::GetAccuracy();
		std::string accuracyStr = accuracyStream.str() + "%";
		ImVec2 accuracySize = ImGui::CalcTextSize(accuracyStr.c_str());
		ImGui::GetBackgroundDrawList()->AddText(positionOffset + ImVec2(windowSize.x - accuracySize.x - 10, scoreSize.y + 10), ImColor(255, 255, 255, 255), accuracyStr.c_str());
		ImGui::PopFont();

		auto progress = time < 0 ? 1.f : std::clamp((duration - time) / static_cast<float>(duration), 0.f, 1.f);
		ImGui::GetBackgroundDrawList()->AddRectFilled(positionOffset + ImVec2(windowSize.x - scoreSize.x - 10, scoreSize.y + accuracySize.y + 15), positionOffset + ImVec2(windowSize.x - (scoreSize.x * progress) - 10, scoreSize.y + accuracySize.y + 25), ImColor(255, 255, 255, 255), 5);
	}
}
