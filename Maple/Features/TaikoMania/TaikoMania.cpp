#include "TaikoMania.h"

#include <sstream>
#include <iomanip>

#include "../../Configuration/ConfigManager.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../SDK/Player/Player.h"
#include "../../UI/StyleProvider.h"
#include "../../SDK/Scoring/Score.h"

void TaikoMania::RenderPlayfield()
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(clientRect.Min, clientRect.Max, ImColor(0, 0, 0, 255));
	ImGui::GetBackgroundDrawList()->AddRectFilled(playfieldRect.Min - ImVec2(5, 5), playfieldRect.Max + ImVec2(5, 5), ImColor(255, 255, 255, 255));
	ImGui::GetBackgroundDrawList()->AddRectFilled(playfieldRect.Min, playfieldRect.Max, ImColor(0, 0, 0, 255));
}

void TaikoMania::RenderObjects(int time)
{
	const bool isRect = ConfigManager::CurrentConfig.Visuals.TaikoMania.NoteStyle == 0;
	const int stageSpacing = ConfigManager::CurrentConfig.Visuals.TaikoMania.StageSpacing;
	const float scrollSpeed = ConfigManager::CurrentConfig.Visuals.TaikoMania.ScrollSpeed;
	const ImVec4 katsuColour = ConfigManager::CurrentConfig.Visuals.TaikoMania.KatsuColour;
	const ImVec4 donColour = ConfigManager::CurrentConfig.Visuals.TaikoMania.DonColour;

	const float objectRadius = (playfieldRect.GetWidth() / stages.size() - stageSpacing * 2) / 2;
	const auto objectSize = ImVec2(objectRadius * 2, isRect ? objectRadius : objectRadius * 2);
	const float scrollDistance = (playfieldRect.GetHeight() - objectSize.y / 2 - stageSpacing - playfieldRect.GetHeight() / 30) - (playfieldRect.Min.y - objectSize.y / 2);

	for (size_t i = 0; i < stages.size(); i++)
	{
		const float xCenter = playfieldRect.Min.x + stageSpacing + i * (playfieldRect.GetWidth() / 4) + objectSize.x / 2;

		for (auto objectTime : stages[i].GetObjects())
		{
			if (time >= objectTime || time < objectTime - (SCROLL_DURATION * 1.2f) / scrollSpeed)
				continue;

			const float t = (time - (objectTime - SCROLL_DURATION / scrollSpeed)) / (SCROLL_DURATION / scrollSpeed);
			const float yCenter = (playfieldRect.Min.y - objectSize.y / 2) + scrollDistance * t;

			if (isRect)
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(xCenter - objectSize.x / 2, yCenter - objectSize.y / 2), ImVec2(xCenter + objectSize.x / 2, yCenter + objectSize.y / 2), stages[i].GetType() == TaikoManiaObjectType::Don ? ImColor(donColour) : ImColor(katsuColour), 5);
			else
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(xCenter, yCenter), objectRadius, stages[i].GetType() == TaikoManiaObjectType::Don ? ImColor(donColour) : ImColor(katsuColour), 32);
		}
	}

	for (size_t i = 0; i < stages.size(); i++)
	{
		if (isRect)
			ImGui::GetBackgroundDrawList()->AddRect(playfieldRect.Min + ImVec2((playfieldRect.GetWidth() / stages.size()) * i + stageSpacing, playfieldRect.GetHeight() - objectSize.y - stageSpacing - playfieldRect.GetHeight() / 30), playfieldRect.Min + ImVec2((playfieldRect.GetWidth() / stages.size()) * i + objectSize.x + stageSpacing, playfieldRect.GetHeight() - stageSpacing - playfieldRect.GetHeight() / 30), ImColor(255, 255, 255, 255), 5, 0, 3);
		else
			ImGui::GetBackgroundDrawList()->AddCircle(playfieldRect.Min + ImVec2((playfieldRect.GetWidth() / stages.size()) * i + objectRadius + stageSpacing, playfieldRect.GetHeight() - objectRadius - stageSpacing - playfieldRect.GetHeight() / 30), objectRadius, ImColor(255, 255, 255, 255), 32, 3);
	}
}

void TaikoMania::RenderStatistics(int time)
{
	ImGui::PushFont(StyleProvider::FontEnormousBold);
	std::ostringstream scoreStream;
	scoreStream << std::setfill('0') << std::setw(8) << Score::GetScore();
	const ImVec2 scoreSize = ImGui::CalcTextSize(scoreStream.str().c_str());
	ImGui::GetBackgroundDrawList()->AddText(clientRect.Min + ImVec2(clientRect.GetWidth() - scoreSize.x - 10, 10), ImColor(255, 255, 255, 255), scoreStream.str().c_str());
	ImGui::PopFont();

	ImGui::PushFont(StyleProvider::FontVeryHugeSemiBold);
	std::ostringstream accuracyStream;
	accuracyStream << std::fixed << std::setprecision(2) << Score::GetAccuracy();
	const auto accuracyStr = accuracyStream.str() + "%";
	const ImVec2 accuracySize = ImGui::CalcTextSize(accuracyStr.c_str());
	ImGui::GetBackgroundDrawList()->AddText(clientRect.Min + ImVec2(clientRect.GetWidth() - accuracySize.x - 10, scoreSize.y + 10), ImColor(255, 255, 255, 255), accuracyStr.c_str());

	if (const int combo = Score::GetCombo(); combo > 0)
	{
		const auto comboStr = std::to_string(Score::GetCombo());
		const ImVec2 comboSize = ImGui::CalcTextSize(comboStr.c_str());
		ImGui::GetBackgroundDrawList()->AddText(playfieldRect.Min + ImVec2(playfieldRect.GetWidth() / 2 - comboSize.x / 2, playfieldRect.GetHeight() / 5 + comboSize.y / 2), ImColor(255, 255, 255, 255), comboStr.c_str());
	}
	ImGui::PopFont();

	const float progress = time < 0 ? 1.f : std::clamp((beatmapDuration - time) / static_cast<float>(beatmapDuration), 0.f, 1.f);
	ImGui::GetBackgroundDrawList()->AddRectFilled(clientRect.Min + ImVec2(clientRect.GetWidth() - scoreSize.x - 10, scoreSize.y + accuracySize.y + 15), clientRect.Min + ImVec2(clientRect.GetWidth() - (scoreSize.x * progress) - 10, scoreSize.y + accuracySize.y + 25), ImColor(255, 255, 255, 255), 5);
}

void TaikoMania::Initialize()
{
	if (!ConfigManager::CurrentConfig.Visuals.TaikoMania.Enabled || Player::GetPlayMode() != PlayModes::Taiko)
		return;

	const Vector2 clientPosition = GameBase::GetClientPosition();
	const Vector2 clientSize = GameBase::GetClientSize();
	const auto playfieldSize = ImVec2(clientSize.X / 4, clientSize.Y);

	clientRect = ImRect(ImVec2(clientPosition.X, clientPosition.Y), ImVec2(clientPosition.X + clientSize.X, clientPosition.Y + clientSize.Y));
	playfieldRect = ImRect(clientRect.Min + ImVec2(clientSize.X / 2 - playfieldSize.x / 2, 0), clientRect.Min + ImVec2(clientSize.X / 2 + playfieldSize.x / 2, playfieldSize.y));

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
		else if (hitObject.IsType(HitObjectType::Slider))
		{
			bool endpointHittable = true;

			if (i < HitObjectManager::GetHitObjectsCount() - 1)
			{
				auto nextHitObject = HitObjectManager::GetHitObject(i + 1);
				int hittableStartTime = nextHitObject.StartTime - hitObject.MinHitDelay;

				if (hittableStartTime - (hitObject.EndTime + hitObject.MinHitDelay) < hitObject.MinHitDelay)
					endpointHittable = false;
			}

			int hittableEndTime = hitObject.EndTime + (endpointHittable ? hitObject.MinHitDelay : 0);

			int count = 0;
			for (int j = hitObject.StartTime; j < hittableEndTime; j += hitObject.MinHitDelay)
			{
				if (count++ % 2)
					primaryDonStage.AddObject(j);
				else
					secondaryDonStage.AddObject(j);
			}
		}
		else if (hitObject.IsType(HitObjectType::Spinner))
		{
			int length = hitObject.EndTime - hitObject.StartTime;
			int hitRate = length / (hitObject.RotationRequirement + 1);
			int count = 0;

			for (int j = hitObject.StartTime; j < hitObject.EndTime; j += hitRate)
			{
				switch (count % 4)
				{
					case 0:
						primaryKatsuStage.AddObject(j);
						break;
					case 1:
						primaryDonStage.AddObject(j);
						break;
					case 2:
						secondaryKatsuStage.AddObject(j);
						break;
					case 3:
						secondaryDonStage.AddObject(j);
						break;
					default:
						break;
				}

				if (++count > hitObject.RotationRequirement + 1)
					break;
			}
		}

		beatmapDuration = hitObject.EndTime;
	}

	stages = { primaryKatsuStage, secondaryKatsuStage, primaryDonStage, secondaryDonStage };
}

void TaikoMania::Render()
{
	if (ConfigManager::CurrentConfig.Visuals.TaikoMania.Enabled && Player::GetIsLoaded() && !Player::GetIsReplayMode() && Player::GetPlayMode() == PlayModes::Taiko && !AudioEngine::GetIsPaused())
	{
		const int time = AudioEngine::GetTime();

		RenderPlayfield();
		RenderObjects(time);
		RenderStatistics(time);
	}
}
