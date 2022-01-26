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

#include <cmath>
#include "../../Sdk/Osu/WindowManager.h"

void AimAssist::DrawDebugOverlay()
{
	if (Config::AimAssist::Enabled && Config::AimAssist::DrawDebugOverlay && Player::IsLoaded() && !Player::IsReplayMode())
	{
		Vector2 viewportPosition = WindowManager::ViewportPosition();
		ImVec2 positionOffset = ImVec2(viewportPosition.X, viewportPosition.Y);

		if (!assistedPosition.IsNull && canAssist)
		{
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();

			// Draw circle on *actual* cursor position
			drawList->AddCircleFilled(positionOffset + ImVec2(rawPosition.X, rawPosition.Y), 12.f, ImColor(StyleProvider::AccentColour));

			// Draw FOV
			drawList->AddCircleFilled(positionOffset + ImVec2(rawPosition.X, rawPosition.Y), distanceScaled * GameField::GetRatio(), ImGui::ColorConvertFloat4ToU32(ImVec4(150.f, 219.f, 96.f, 0.4f)));

			// Draw Sliderball position
			if (Config::AimAssist::Algorithm == 0)
			{
				Vector2 screen2 = GameField::FieldToDisplay(sliderBallPos);
				drawList->AddCircleFilled(positionOffset + ImVec2(screen2.X, screen2.Y), Config::AimAssist::SliderballDeadzone * 2.f, decided ? ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 255.f, 0.f, 0.5f)) : ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 0.f, 0.f, 0.5f)));
			}

			// Draw small debug box uwu
			ImGui::PushFont(StyleProvider::FontSmallBold);
			std::string positionString = "(" + std::to_string((int)assistedPosition.X) + "; " + std::to_string((int)assistedPosition.Y) + ")";
			drawList->AddRectFilled(positionOffset + ImVec2(assistedPosition.X + 20, assistedPosition.Y + 20), positionOffset + ImVec2(assistedPosition.X + 20, assistedPosition.Y + 20) + ImGui::CalcTextSize(positionString.c_str()) + StyleProvider::Padding * 2, ImColor(StyleProvider::MenuColourDark), 10.f);
			drawList->AddText(positionOffset + ImVec2(assistedPosition.X + 20, assistedPosition.Y + 20) + StyleProvider::Padding, ImColor(255.f, 255.f, 255.f, 255.f), positionString.c_str());
			ImGui::PopFont();
		}
	}
}

Vector2 AimAssist::doAssist(Vector2 realPosition)
{
	if (!Config::AimAssist::Enabled || !Player::IsLoaded() || !canAssist)
		return realPosition;

	const float strength = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength / 2.f < 0.7f ? Config::AimAssist::EasyModeStrength / 2.f : (Config::AimAssist::EasyModeStrength / 2.f) - 0.214f : Config::AimAssist::Strength;
	const int baseFOV = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 50.f : Config::AimAssist::BaseFOV;
	const float maximumFOVScale = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 2.f + .25f : Config::AimAssist::MaximumFOVScale;
	const float minimumFOVTotal = Config::AimAssist::EasyMode ? 0 : Config::AimAssist::MinimumFOVTotal;
	const float maximumFOVTotal = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 220 : Config::AimAssist::MaximumFOVTotal;
	const bool assistOnSliders = Config::AimAssist::EasyMode ? true : Config::AimAssist::AssistOnSliders;
	const bool flipSliderballDeadzone = Config::AimAssist::EasyMode ? false : Config::AimAssist::FlipSliderballDeadzone;
	const float sliderballDeadzone = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 12.2f + 2.1f : Config::AimAssist::SliderballDeadzone;
	const float strengthMultiplier = Config::AimAssist::EasyMode ? 1.f : Config::AimAssist::StrengthMultiplier;
	const float assistDeadzone = Config::AimAssist::EasyMode ? 3.f : Config::AimAssist::AssistDeadzone;
	const float resyncLeniency = Config::AimAssist::EasyMode ? 3.5f : Config::AimAssist::ResyncLeniency;
	const float resyncLeniencyFactor = Config::AimAssist::EasyMode ? 0.693f : Config::AimAssist::ResyncLeniencyFactor;

	const int time = AudioEngine::Time();
	if (time > (currentHitObject.IsType(HitObjectType::Slider) && assistOnSliders ? currentHitObject.EndTime : currentHitObject.StartTime))
	{
		currentIndex++;

		if (currentIndex >= HitObjectManager::GetHitObjectsCount())
		{
			canAssist = false;

			return realPosition;
		}

		previousHitObject = currentHitObject;
		currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	}

	rawPosition = realPosition;

	Vector2 playfieldCoords = GameField::DisplayToField(realPosition);

	const auto arScale = std::clamp(
		std::max(0.f, ((AudioEngine::Time() - (currentHitObject.StartTime - hitWindow50)) / static_cast<float>(preEmpt * 3.f) + 1.f)) * 1.4f,
		0.f,
		maximumFOVScale);

	distanceScaled = std::clamp(static_cast<float>(baseFOV) * arScale, minimumFOVTotal, maximumFOVTotal);
	if (!currentHitObject.IsType(HitObjectType::Spinner) && lastPos.Distance(playfieldCoords) >= assistDeadzone)
	{
		if (currentHitObject.IsType(HitObjectType::Slider))
		{
			sliderBallPos = time >= currentHitObject.EndTime ? currentHitObject.PositionAtTime(currentHitObject.EndTime - 1) : currentHitObject.PositionAtTime(time);
			if (sliderBallPos != currentHitObject.Position)
				if (flipSliderballDeadzone)
					decided = InCircle(sliderBallPos, sliderballDeadzone, playfieldCoords);
				else
					decided = !InCircle(sliderBallPos, sliderballDeadzone, playfieldCoords);
			else
				decided = true;
		}
		else
			decided = true;
	}

	if (decided)
	{
		if (!previousHitObject.IsNull)
		{
			const float diffobj = currentHitObject.StartTime - (assistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.EndTime : previousHitObject.StartTime);
			if (diffobj > 0.001f)
			{
				const float fromobj = currentHitObject.StartTime - time;
				float t = fromobj / diffobj;
				t = std::clamp(t, 0.0f, 1.0f);

				playfieldCoords = Algorithmv0(t * strength, distanceScaled, assistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.PositionAtTime(previousHitObject.EndTime - 1) : previousHitObject.Position, playfieldCoords, strengthMultiplier);

				Vector2 toAssist = currentHitObject.Position;
				if (assistOnSliders && currentHitObject.IsType(HitObjectType::Slider))
					if (time > currentHitObject.EndTime)
						toAssist = currentHitObject.PositionAtTime(currentHitObject.EndTime - 1);
					else
						toAssist = currentHitObject.PositionAtTime(time);

				playfieldCoords = Algorithmv0((1.0f - t) * strength, distanceScaled, toAssist, playfieldCoords, strengthMultiplier);
			}
		}
		else
		{
			const float diffobj = 1000.0f;
			const float fromobj = currentHitObject.StartTime - time;
			if (fromobj > 0.0f)
			{
				float t = fromobj / diffobj;
				t = std::clamp(t, 0.0f, 1.0f);

				Vector2 toAssist = currentHitObject.Position;
				if (assistOnSliders && currentHitObject.IsType(HitObjectType::Slider))
					if (time > currentHitObject.EndTime)
						toAssist = currentHitObject.PositionAtTime(currentHitObject.EndTime - 1);
					else
						toAssist = currentHitObject.PositionAtTime(time);

				playfieldCoords = Algorithmv0((1.0f - t) * strength, distanceScaled, toAssist, playfieldCoords, strengthMultiplier);
			}
		}
	}

	if (lastPos.Distance(playfieldCoords) > (resyncLeniency * 0.8f) && lastPos != Vector2(0, 0))
	{
		Vector2 offset = lastPos - playfieldCoords;

		const float factor = resyncLeniencyFactor - 0.102f;

		playfieldCoords = playfieldCoords + ((offset * factor) * 0.9f);
	}

	lastPos = playfieldCoords;
	assistedPosition = GameField::FieldToDisplay(playfieldCoords);
	return assistedPosition;
}

static auto __forceinline calc_fov_scale(float t, float begin, float hit_window_50, float pre_empt, float magnitude = 1.4f, float max = 2.5f) {
	return _mm_min_ps(
		_mm_max_ps(
			_mm_mul_ps(
				_mm_add_ps(
					_mm_div_ps(_mm_sub_ps(_mm_load_ps(&begin), _mm_load_ps(&hit_window_50)), _mm_mul_ps(_mm_load_ps(&pre_empt), _mm_set_ps1(3.f))),
					_mm_set_ps1(1.f)),
				_mm_load_ps(&magnitude)),
			_mm_set_ps1(0.f)),
		_mm_set_ps1(max))
		.m128_f32[0];
}

static auto __forceinline point_in_radius(const Vector2& point, const Vector2& anchor, float radius) {
	return _mm_sqrt_ps(_mm_add_ps(
		_mm_pow_ps(_mm_sub_ps(_mm_load_ps(&anchor.X), _mm_load_ps(&point.X)), _mm_set_ps1(2.f)),
		_mm_pow_ps(_mm_sub_ps(_mm_load_ps(&anchor.Y), _mm_load_ps(&point.Y)), _mm_set_ps1(2.f))))
		.m128_f32[0] <= radius;
}

static auto __forceinline _mm_abs_ps(__m128 _A) {
	return _mm_andnot_ps(_mm_set1_ps(-0.0f), _A);
}

static auto __forceinline calc_interpolant(const Vector2& window_size, float displacement, float strength) {
	return _mm_min_ps(
		_mm_set_ps1(1.f),
		_mm_mul_ps(
			_mm_abs_ps(_mm_div_ps(
				_mm_load_ps(&displacement), _mm_div_ps(_mm_min_ps(_mm_load_ps(&window_size.X), _mm_load_ps(&window_size.Y)), _mm_set_ps1(2.f)))),
			_mm_load_ps(&strength)))
		.m128_f32[0];
}

Vector2 AimAssist::doAssistv2(Vector2 realPosition)
{
	if (!Config::AimAssist::Enabled || !Player::IsLoaded() || !canAssist)
		return realPosition;

	const int time = AudioEngine::Time();
	if (time > currentHitObject.EndTime)
	{
		currentIndex++;

		if (currentIndex >= HitObjectManager::GetHitObjectsCount())
		{
			canAssist = false;

			return realPosition;
		}

		previousHitObject = currentHitObject;
		currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	}

	rawPosition = realPosition;

	Vector2 hitObjectPosition = GameField::FieldToDisplay(Config::AimAssist::Algorithmv2AssistOnSliders ? currentHitObject.PositionAtTime(time) : currentHitObject.Position);

	Vector2 distance = hitObjectPosition - realPosition;
	auto fov = (40.f * Config::AimAssist::Algorithmv2Power);
	const auto clamp_range = Config::AimAssist::Algorithmv2Power * 16.f; // * 8.f

	if (!currentHitObject.IsType(HitObjectType::Spinner) && !Player::IsPaused())
	{
		if (point_in_radius(realPosition, hitObjectPosition, calc_fov_scale(time, currentHitObject.StartTime - hitWindow50, hitWindow50, preEmpt) * fov)) {
			if (!point_in_radius(realPosition, lastPos, 1.75f) && Config::AimAssist::Algorithmv2Power && !previousHitObject.IsNull) {
				const auto interpolant = calc_interpolant(windowSize, distance.Length(), Config::AimAssist::Algorithmv2Power);

				if (interpolant > std::numeric_limits<float>::epsilon()) {
					offset.X = std::clamp(std::lerp(offset.X, distance.X, interpolant), -(Config::AimAssist::Algorithmv2Power * 16.f), Config::AimAssist::Algorithmv2Power * 16.f);
					offset.Y = std::clamp(std::lerp(offset.Y, distance.Y, interpolant), -(Config::AimAssist::Algorithmv2Power * 16.f), Config::AimAssist::Algorithmv2Power * 16.f);
				}
			}
		}
		else if (offset.Length() > std::numeric_limits<float>::epsilon()) {
			const auto dt = ImGui::GetIO().DeltaTime;

			if (auto delta = lastPos - realPosition; delta.Length() > std::numeric_limits<float>::epsilon()) {
				if (auto change = (delta / offset) * dt; std::isfinite(change.Length())) {
					auto dpi = delta * (offset.Length() / clamp_range) * .15f;

					offset.X = change.X < 0.f ? offset.X + dpi.X : offset.X - dpi.X;
					offset.Y = change.Y < 0.f ? offset.Y + dpi.Y : offset.Y - dpi.Y;
				}
			}
		}
	}

	lastPos = realPosition;
	assistedPosition = realPosition + offset;
	return assistedPosition;

}

Vector2 AimAssist::doAssistv3(Vector2 realPosition)
{
	if (!Config::AimAssist::Enabled || !Player::IsLoaded() || !canAssist)
		return realPosition;

	const int time = AudioEngine::Time();
	if (time > (currentHitObject.IsType(HitObjectType::Slider) && Config::AimAssist::Algorithmv3AssistOnSliders ? currentHitObject.EndTime : currentHitObject.StartTime))
	{
		currentIndex++;

		if (currentIndex >= HitObjectManager::GetHitObjectsCount())
		{
			canAssist = false;

			return realPosition;
		}

		previousHitObject = currentHitObject;
		currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	}

	rawPosition = realPosition;

	Vector2 cursorPosition = GameField::DisplayToField(realPosition);
	Vector2 hitObjectPosition = Config::AimAssist::Algorithmv3AssistOnSliders ? currentHitObject.PositionAtTime(time) : currentHitObject.Position;

	const auto arScale = std::clamp(
		std::max(0.f, ((AudioEngine::Time() - (currentHitObject.StartTime - hitWindow50)) / static_cast<float>(preEmpt * 3.f) + 1.f)) * 1.4f,
		0.f,
		Config::AimAssist::Algorithmv3MaximumFOVScale);

	distanceScaled = std::clamp(static_cast<float>(Config::AimAssist::Algorithmv3BaseFOV) * arScale, Config::AimAssist::Algorithmv3MinimumFOVTotal, Config::AimAssist::Algorithmv3MaximumFOVTotal);
	if (!currentHitObject.IsType(HitObjectType::Spinner) && !Player::IsPaused())
	{
		if (hitObjectPosition.Distance(cursorPosition) <= distanceScaled || (!previousHitObject.IsNull && (Config::AimAssist::Algorithmv3AssistOnSliders ? previousHitObject.EndPosition : previousHitObject.Position).Distance(cursorPosition) <= distanceScaled))
		{
			if (lastPos.Distance(cursorPosition) >= Config::AimAssist::Algorithmv3AccelerationFactor)
			{
				if (!previousHitObject.IsNull)
				{
					const float diffobj = std::min(preEmpt, currentHitObject.StartTime - (Config::AimAssist::Algorithmv3AssistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.EndTime : previousHitObject.StartTime));
					const float fromobj = currentHitObject.StartTime - time;
					const float t = std::clamp(fromobj / diffobj, 0.f, 1.f);

					offset = Algorithmv3((1.0f - t) * Config::AimAssist::Algorithmv3Strength, distanceScaled, hitObjectPosition, cursorPosition, offset);
				}
				else
				{
					const float diffobj = 1000.f;
					const float fromobj = currentHitObject.StartTime - time;
					const float t = std::clamp(fromobj / diffobj, 0.f, 1.f);

					offset = Algorithmv3((1.0f - t) * Config::AimAssist::Algorithmv3Strength, distanceScaled, hitObjectPosition, cursorPosition, offset);
				}
			}
		}
		else if (offset.Length() > std::numeric_limits<float>::epsilon())
		{
			const auto dt = ImGui::GetIO().DeltaTime;
			if (auto delta = lastPos - cursorPosition; delta.Length() > std::numeric_limits<float>::epsilon())
			{
				if (auto change = (delta / offset) * dt; std::isfinite(change.Length()))
				{
					auto dpi = delta * (offset.Length() / (Config::AimAssist::Algorithmv3Strength * 8.f)) * .15f;

					offset.X = change.X < 0.f ? offset.X + dpi.X : offset.X - dpi.X;
					offset.Y = change.Y < 0.f ? offset.Y + dpi.Y : offset.Y - dpi.Y;
				}
			}
		}
	}

	lastPos = cursorPosition;
	assistedPosition = GameField::FieldToDisplay(cursorPosition + offset);
	return assistedPosition;
}

void AimAssist::Reset()
{
	hitWindow50 = HitObjectManager::GetHitWindow50();
	preEmpt = HitObjectManager::GetPreEmpt();
	hitObjectRadius = HitObjectManager::GetHitObjectRadius();
	
	currentIndex = HitObjectManager::GetCurrentHitObjectIndex();
	currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	previousHitObject = currentIndex > 0 ? HitObjectManager::GetHitObject(currentIndex) : HitObject();

	canAssist = true;
	
	decided = true;
	lastPos = Vector2(0, 0);

	windowSize = Vector2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	offset = Vector2();
}

void __stdcall AimAssist::UpdateCursorPosition(float x, float y)
{
	const Vector2 assistedPosition = Config::AimAssist::Algorithm == 0 ? doAssist(Vector2(x, y)) : Config::AimAssist::Algorithm == 1 ? doAssistv2(Vector2(x, y)) : doAssistv3(Vector2(x, y));

	oUpdateCursorPosition(assistedPosition.X, assistedPosition.Y);
}

bool AimAssist::InCircle(Vector2 circle, float radius, Vector2 point)
{
	float x = std::powf(point.X - circle.X, 2);
	float y = std::powf(point.Y - circle.Y, 2);
	float rad = std::powf(radius, 2);

	return x + y < rad;
}

Vector2 AimAssist::Algorithmv0(float strength, float distance, Vector2 hitObjectPosition, Vector2 cursorPosition, float strengthMultiplier)
{
	float dist = hitObjectPosition.Distance(cursorPosition);
	if (dist > distance)
		return cursorPosition;

	float t = (1.0f - (dist / distance)) * strength;
	t = std::clamp(t, 0.f, 1.f);
	return cursorPosition + ((hitObjectPosition - cursorPosition) * std::clamp(t, 0.f,
		std::clamp(t * strengthMultiplier, 0.f, 1.f)));
}

Vector2 AimAssist::Algorithmv3(float strength, float distance, Vector2 hitObjectPosition, Vector2 cursorPosition, Vector2 offset)
{
	float dist = hitObjectPosition.Distance(cursorPosition);
	if (dist > distance)
		return offset;

	float t = (1.0f - (dist / distance)) * strength;
	t = std::clamp(t, 0.f, 1.f);

	return offset + ((hitObjectPosition - cursorPosition) * t - offset) * strength;
}
