#include "AimAssist.h"
#define NOMINMAX

#include "../../Config/Config.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Osu/GameField.h"
#include "../../Sdk/Player/Player.h"

#include "../../Dependencies/ImGui/imgui.h"
#include "../../UI/StyleProvider.h"

void AimAssist::DrawDebugOverlay()
{
	if (Config::AimAssist::DrawDebugOverlay)
	{
		// Draw small debug box uwu
		ImGui::SetNextWindowSize(ImVec2(GameBase::GetClientBounds()->Width, GameBase::GetClientBounds()->Height));
		ImGui::Begin("aa", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			if (!AssistedPosition.IsNull && CanAssist)
			{
				std::string assistX = "X:" + std::to_string(AssistedPosition.X);
				std::string assistY = "Y:" + std::to_string(AssistedPosition.Y);
				drawList->AddRectFilled(ImVec2(AssistedPosition.X + 30, AssistedPosition.Y + 30), ImVec2(AssistedPosition.X + 110, AssistedPosition.Y + 70), ImGui::ColorConvertFloat4ToU32(ImVec4(227.f, 227.f, 227.f, 1.f)), 2.f);

				ImGui::PushFont(StyleProvider::FontSmallBold);
				ImGui::SetCursorPos(ImVec2(AssistedPosition.X - 28, AssistedPosition.Y - 28));
				ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), assistX.c_str());
				ImGui::SetCursorPos(ImVec2(AssistedPosition.X - 28, AssistedPosition.Y - 13));
				ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), assistY.c_str());
				ImGui::PopFont();

				// Draw circle on *actual* cursor position
				drawList->AddCircleFilled(ImVec2(RawPosition.X, RawPosition.Y), 12.f, ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 255.f, 0.f, 0.6f)));

				// Draw FOV
				drawList->AddCircleFilled(ImVec2(AssistedPosition.X, AssistedPosition.Y), distanceScaled, ImGui::ColorConvertFloat4ToU32(ImVec4(150.f, 219.f, 96.f, 0.4f)));

				// Draw Last position
				Vector2 screen = GameField::FieldToDisplay(lastPos);
				drawList->AddCircleFilled(ImVec2(screen.X, screen.Y), 20.f, ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 111.f, 70.f, 0.4f)));

				// Draw Sliderball position
				Vector2 screen2 = GameField::FieldToDisplay(sliderBallPos);
				drawList->AddCircleFilled(ImVec2(screen2.X, screen2.Y), Config::AimAssist::SliderballDeadzone * 2, decided ? ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 255.f, 0.f, 0.5f)) : ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 0.f, 0.f, 0.5f)));
			}
		}
		ImGui::End();
	}
}

void AimAssist::AssistThread()
{
	while (true)
	{
		if (Config::AimAssist::Enabled) {
			if (!RawPosition.IsNull)
				AssistedPosition = DoAssist(RawPosition);
		}
		else
			AssistedPosition = RawPosition;

		Sleep(1);
	}
}

Vector2 AimAssist::DoAssist(Vector2 realPosition)
{
	if (!Player::IsLoaded() && IsLoaded)
	{
		Reset();

		return realPosition;
	}

	if (!Player::IsLoaded())
		return realPosition;

	if (Player::IsLoaded() && !IsLoaded)
		Load();

	if (!CanAssist)
		return realPosition;

	int time = AudioEngine::Time();
	if (time > (cachedHitObjects[noteIndex].IsType(HitObjectType::Slider) && Config::AimAssist::AssistOnSliders 
		? cachedHitObjects[noteIndex].EndTime : cachedHitObjects[noteIndex].StartTime))
		noteIndex++;

	if (noteIndex >= HitObjectManager::GetHitObjectsCount())
	{
		CanAssist = false;

		return realPosition;
	}

	HitObject currentHitObject = cachedHitObjects[noteIndex];

	Vector2 playfieldCoords = GameField::DisplayToField(realPosition);

	const auto arScale = std::clamp(
		std::max(0.f, ((AudioEngine::Time() - (currentHitObject.StartTime - HitObjectManager::GetHitWindow50())) / static_cast<float>((HitObjectManager::GetPreEmpt()) * 3.f) + 1.f)) * 1.4f,
		0.f,
		Config::AimAssist::MaximumFOVScale);

	distanceScaled = std::clamp(static_cast<float>(Config::AimAssist::BaseFOV) * arScale, Config::AimAssist::MinimumFOVTotal, Config::AimAssist::MaximumFOVTotal);
	if (!currentHitObject.IsType(HitObjectType::Spinner) && lastPos.Distance(playfieldCoords) >= Config::AimAssist::AssistDeadzone) {
		if (currentHitObject.IsType(HitObjectType::Slider)) {
			sliderBallPos = time >= currentHitObject.EndTime ? currentHitObject.PositionAtTime(currentHitObject.EndTime - 1) : currentHitObject.PositionAtTime(time);
			if (sliderBallPos != currentHitObject.Position)
				if (Config::AimAssist::FlipSliderballDeadzone)
					decided = InCircle(sliderBallPos, Config::AimAssist::SliderballDeadzone, playfieldCoords);
				else
					decided = !InCircle(sliderBallPos, Config::AimAssist::SliderballDeadzone, playfieldCoords);
			else
				decided = true;
		}
		else
			decided = true;
	}

		if (decided)
			if (noteIndex)
			{
				HitObject previousHitObject = cachedHitObjects[noteIndex - 1];
				float diffobj = currentHitObject.StartTime - (Config::AimAssist::AssistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.EndTime : previousHitObject.StartTime);
				if (diffobj > 0.001f)
				{
					float fromobj = currentHitObject.StartTime - time;
					float t = fromobj / diffobj;
					t = std::clamp(t, 0.0f, 1.0f);

					playfieldCoords = Algorithmv0(t * Config::AimAssist::Strength, distanceScaled, Config::AimAssist::AssistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.PositionAtTime(previousHitObject.EndTime - 1) : previousHitObject.Position, playfieldCoords);

					Vector2 toAssist = currentHitObject.Position;
					if (Config::AimAssist::AssistOnSliders && currentHitObject.IsType(HitObjectType::Slider))
						if (time > currentHitObject.EndTime)
							toAssist = currentHitObject.PositionAtTime(currentHitObject.EndTime - 1);
						else
							toAssist = currentHitObject.PositionAtTime(time);

					playfieldCoords = Algorithmv0((1.0f - t) * Config::AimAssist::Strength, distanceScaled, toAssist, playfieldCoords);
				}
			}
			else
			{
				float diffobj = 1000.0f;
				float fromobj = currentHitObject.StartTime - time;
				if (fromobj > 0.0f)
				{
					float t = fromobj / diffobj;
					t = std::clamp(t, 0.0f, 1.0f);

					Vector2 toAssist = currentHitObject.Position;
					if (Config::AimAssist::AssistOnSliders && currentHitObject.IsType(HitObjectType::Slider))
						if (time > currentHitObject.EndTime)
							toAssist = currentHitObject.PositionAtTime(currentHitObject.EndTime - 1);
						else
							toAssist = currentHitObject.PositionAtTime(time);

					playfieldCoords = Algorithmv0((1.0f - t) * Config::AimAssist::Strength, distanceScaled, toAssist, playfieldCoords);
				}
			}

	if (lastPos.Distance(playfieldCoords) > (Config::AimAssist::ResyncLeniency * 0.8f) && lastPos != Vector2(0, 0))
	{
		Vector2 offset = lastPos - playfieldCoords;

		float factor = Config::AimAssist::ResyncLeniencyFactor - 0.102f;

		playfieldCoords = playfieldCoords + ((offset * factor) * 0.9f);
	}

	lastPos = playfieldCoords;
	return GameField::FieldToDisplay(playfieldCoords);
}

void AimAssist::Load()
{
	cachedHitObjects = HitObjectManager::GetAllHitObjects();

	IsLoaded = true;
	CanAssist = true;
}

void AimAssist::Reset()
{
	decided = true;
	noteIndex = 0;
	lastPos = Vector2(0, 0);
	cachedHitObjects.clear();

	IsLoaded = false;
	CanAssist = false;
}

void __stdcall AimAssist::UpdateCursorPosition(float x, float y)
{
	RawPosition = Vector2(x, y);
	if (AssistedPosition.IsNull)
		oUpdateCursorPosition(x, y);
	else
		oUpdateCursorPosition(AssistedPosition.X, AssistedPosition.Y);
}

bool AimAssist::InCircle(Vector2 circle, float radius, Vector2 point)
{
	float x = std::powf(point.X - circle.X, 2);
	float y = std::powf(point.Y - circle.Y, 2);
	float rad = std::powf(radius, 2);

	return x + y < rad;
}

Vector2 AimAssist::Algorithmv0(float strength, float distance, Vector2 hitObjectPosition, Vector2 cursorPosition)
{
	float dist = hitObjectPosition.Distance(cursorPosition);
	if (dist > distance)
		return cursorPosition;

	float t = (1.0f - (dist / distance)) * strength;
	t = std::clamp(t, 0.f, 1.f);
	return cursorPosition + ((hitObjectPosition - cursorPosition) * std::clamp(t, 0.f,
		std::clamp(t * Config::AimAssist::StrengthMultiplier, 0.f, 1.f)));
}
