#include "AimAssist.h"

#include <intrin.h>

#define NOMINMAX
#include <cmath>

#include "../../SDK/Input/InputManager.h"
#include "../../Config/Config.h"
#include "../../SDK/Osu/GameField.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Player/Player.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../UI/StyleProvider.h"

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

Vector2 AimAssist::algorithmv1(Vector2 pos)
{
	const int time = AudioEngine::GetTime();

	Vector2 previousHitObjectPosition = GameField::FieldToDisplay(previousHitObject.IsNull ? Vector2() : Config::AimAssist::Algorithmv1::AssistOnSliders ? previousHitObject.EndPosition : previousHitObject.Position);
	Vector2 currentHitObjectPosition = GameField::FieldToDisplay(Config::AimAssist::Algorithmv1::AssistOnSliders ? currentHitObject.PositionAtTime(time) : currentHitObject.Position);

	const float previousHitObjectDistance = previousHitObject.IsNull ? 0.f : previousHitObjectPosition.Distance(pos);
	const float currentHitObjectDistance = currentHitObjectPosition.Distance(pos);

	const auto arScale = std::clamp(
		std::max(0.f, ((AudioEngine::GetTime() - (currentHitObject.StartTime - hitWindow50)) / static_cast<float>(preEmpt * 3.f) + 1.f)) * 1.4f,
		0.f,
		Config::AimAssist::Algorithmv1::MaximumFOVScale);

	fov = std::clamp(static_cast<float>(Config::AimAssist::Algorithmv1::BaseFOV) * arScale, Config::AimAssist::Algorithmv1::MinimumFOVTotal, Config::AimAssist::Algorithmv1::MaximumFOVTotal) * GameField::GetRatio();
	if (!currentHitObject.IsType(HitObjectType::Spinner))
	{
		if (currentHitObjectDistance <= fov || (!previousHitObject.IsNull && previousHitObjectDistance <= fov))
		{
			if (InputManager::GetLastCursorPosition().Distance(pos) >= Config::AimAssist::Algorithmv1::AccelerationFactor * GameField::GetRatio())
			{
				const float diffobj = previousHitObject.IsNull ? preEmpt : currentHitObject.StartTime - (Config::AimAssist::Algorithmv1::AssistOnSliders && previousHitObject.IsType(HitObjectType::Slider) ? previousHitObject.EndTime : previousHitObject.StartTime);
				const float fromobj = currentHitObject.StartTime - time;
				const float t = std::clamp(fromobj / diffobj, 0.f, 1.f);

				const float previousInterpolant = (1.0f - (previousHitObjectDistance / fov)) * (t * Config::AimAssist::Algorithmv1::Strength);
				const float interpolant = (1.f - (currentHitObjectDistance / fov)) * ((1.f - t) * Config::AimAssist::Algorithmv1::Strength);

				Vector2 previousOffset = Vector2(0, 0);
				if (!previousHitObject.IsNull && previousHitObjectDistance <= fov)
					previousOffset = (previousHitObjectPosition - pos) * previousInterpolant;

				if (currentHitObjectDistance <= fov)
					InputManager::SetAccumulatedOffset(InputManager::GetAccumulatedOffset() + ((currentHitObjectPosition - pos) * interpolant + previousOffset - InputManager::GetAccumulatedOffset()) * interpolant);
				else
					InputManager::SetAccumulatedOffset(InputManager::GetAccumulatedOffset() + (previousOffset - InputManager::GetAccumulatedOffset()) * previousInterpolant);
			}
		}
		else
			InputManager::SetAccumulatedOffset(InputManager::Resync(InputManager::GetLastCursorPosition() - pos, InputManager::GetAccumulatedOffset(), .3f * Config::AimAssist::Algorithmv1::Strength));
	}
	
	return pos + InputManager::GetAccumulatedOffset();
}

Vector2 AimAssist::algorithmv2(Vector2 pos)
{
	const int time = AudioEngine::GetTime();

	Vector2 hitObjectPosition = GameField::FieldToDisplay(Config::AimAssist::Algorithmv2::AssistOnSliders ? currentHitObject.PositionAtTime(time) : currentHitObject.Position);

	Vector2 distance = hitObjectPosition - pos;
	fov = 40.f * Config::AimAssist::Algorithmv2::Power;

	if (!currentHitObject.IsType(HitObjectType::Spinner))
	{
		if (point_in_radius(pos, hitObjectPosition, calc_fov_scale(time, currentHitObject.StartTime - hitWindow50, hitWindow50, preEmpt) * fov)) {
			if (!point_in_radius(pos, InputManager::GetLastCursorPosition(), 1.75f) && Config::AimAssist::Algorithmv2::Power && !previousHitObject.IsNull) {
				const auto interpolant = calc_interpolant(windowSize, distance.Length(), Config::AimAssist::Algorithmv2::Power);

				if (interpolant > std::numeric_limits<float>::epsilon())
					InputManager::SetAccumulatedOffset(Vector2(std::clamp(std::lerp(InputManager::GetAccumulatedOffset().X, distance.X, interpolant), -(Config::AimAssist::Algorithmv2::Power * 16.f), Config::AimAssist::Algorithmv2::Power * 16.f), std::clamp(std::lerp(InputManager::GetAccumulatedOffset().Y, distance.Y, interpolant), -(Config::AimAssist::Algorithmv2::Power * 16.f), Config::AimAssist::Algorithmv2::Power * 16.f)));
			}
		}
		else
			InputManager::SetAccumulatedOffset(InputManager::Resync(InputManager::GetLastCursorPosition() - pos, InputManager::GetAccumulatedOffset(), .3f * Config::AimAssist::Algorithmv2::Power));
	}

	return pos + InputManager::GetAccumulatedOffset();
}

Vector2 AimAssist::algorithmv3(Vector2 pos)
{
	const int time = AudioEngine::GetTime();

	const float scaledHitObjectRadius = hitObjectRadius * GameField::GetRatio();

	const float power = Config::AimAssist::Algorithmv3::Power;
	const float half_power = power - 1.f;
	float power_low = power <= 1.f ? power * .7f : (half_power * .15f) + .7f;
	const float power_high = power * 16.f * GameField::GetRatio();
	fov = std::max(1.2f, 4.f * power_low) * scaledHitObjectRadius;

	//aqn:
	/*
	if (power <= 1.f)
	{
		power_high = (power * 8.f) * GameField::GetRatio();
		power_low = power * .35f;
	}
	else
	{
		auto diff = power > 1.f ? std::min(1.f, half_power) : 0.f;
		power_high = (diff + diff + 8.f) * GameField::GetRatio();
		power_low = diff * .15f + .35f;
	}
	 */

	Vector2 hitObjectPosition = GameField::FieldToDisplay(currentHitObject.PositionAtTime(time));

	auto adjustedPos = pos + InputManager::GetAccumulatedOffset();

	auto weight = adjustedPos.Distance(hitObjectPosition) <= scaledHitObjectRadius ? .65f : 1.f;
	auto distance = hitObjectPosition - adjustedPos;
	auto displacement = InputManager::GetLastCursorPosition() -pos;

	if (!currentHitObject.IsType(HitObjectType::Spinner))
	{
		if (pos.Distance(hitObjectPosition) <= fov && time > currentHitObject.StartTime - preEmpt)
		{
			const float in_slider_radius = scaledHitObjectRadius * .35f;
			if (adjustedPos.Distance(hitObjectPosition) <= in_slider_radius && currentHitObject.IsType(HitObjectType::Slider))
				power_low = std::min(distance.Length() / in_slider_radius, 1.f) * power_low;

			if (time > currentHitObject.StartTime && currentHitObject.IsType(HitObjectType::Slider))
				power_low *= Config::AimAssist::Algorithmv3::SliderAssistPower;

			InputManager::SetAccumulatedOffset(Vector2(InputManager::GetAccumulatedOffset().X + (distance.X / distance.Length() * (weight * power_low * std::min(distance.Length(), displacement.Length()))), InputManager::GetAccumulatedOffset().Y + (distance.Y / distance.Length() * (weight * power_low * std::min(distance.Length(), displacement.Length())))));
			InputManager::SetAccumulatedOffset(Vector2(std::clamp(InputManager::GetAccumulatedOffset().X, -power_high, power_high), std::clamp(InputManager::GetAccumulatedOffset().Y, -power_high, power_high)));
		}
		else
			InputManager::SetAccumulatedOffset(InputManager::Resync(displacement, InputManager::GetAccumulatedOffset(), .6f * power_low));
	}
	
	return pos + InputManager::GetAccumulatedOffset();
}

void AimAssist::Initialize()
{
	hitWindow50 = HitObjectManager::GetHitWindow50();
	preEmpt = HitObjectManager::GetPreEmpt();
	hitObjectRadius = HitObjectManager::GetHitObjectRadius();

	currentIndex = HitObjectManager::GetCurrentHitObjectIndex();
	currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	previousHitObject = currentIndex > 0 ? HitObjectManager::GetHitObject(currentIndex) : HitObject();

	windowSize = GameBase::GetClientSize();
}

Vector2 AimAssist::GetCursorPosition(Vector2 pos)
{
	const int time = AudioEngine::GetTime();
	if (time > currentHitObject.EndTime)
	{
		if (currentIndex + 1 >= HitObjectManager::GetHitObjectsCount())
			return pos + InputManager::GetAccumulatedOffset();

		currentIndex++;

		previousHitObject = currentHitObject;
		currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	}

	return Config::AimAssist::Algorithm == 0 ? algorithmv1(pos) : Config::AimAssist::Algorithm == 1 ? algorithmv2(pos) : algorithmv3(pos);
}

void AimAssist::Render()
{
	if (Config::AimAssist::Enabled && Config::AimAssist::DrawDebugOverlay && Player::GetIsLoaded() && !Player::GetIsReplayMode())
	{
		const Vector2 clientPosition = GameBase::GetClientPosition();
		const ImVec2 positionOffset = ImVec2(clientPosition.X, clientPosition.Y);

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		const Vector2 cursorPosition = InputManager::GetCursorPosition();
		const Vector2 realPosition = InputManager::GetLastCursorPosition();

		// Draw circle on *actual* cursor position
		drawList->AddCircleFilled(positionOffset + ImVec2(realPosition.X, realPosition.Y), 12.f, ImColor(StyleProvider::AccentColour));

		// Draw FOV
		drawList->AddCircleFilled(positionOffset + ImVec2(realPosition.X, realPosition.Y), fov * (Config::AimAssist::Algorithm == 1 ? GameField::GetRatio() : 1.f), ImGui::ColorConvertFloat4ToU32(ImVec4(150.f, 219.f, 96.f, 0.4f)));

		// Draw small debug box uwu
		ImGui::PushFont(StyleProvider::FontSmallBold);
		const std::string positionString = "(" + std::to_string((int)cursorPosition.X) + "; " + std::to_string((int)cursorPosition.Y) + ")";
		drawList->AddRectFilled(positionOffset + ImVec2(cursorPosition.X + 20, cursorPosition.Y + 20), positionOffset + ImVec2(cursorPosition.X + 20, cursorPosition.Y + 20) + ImGui::CalcTextSize(positionString.c_str()) + StyleProvider::Padding * 2, ImColor(StyleProvider::MenuColourDark), 10.f);
		drawList->AddText(positionOffset + ImVec2(cursorPosition.X + 20, cursorPosition.Y + 20) + StyleProvider::Padding, ImColor(255.f, 255.f, 255.f, 255.f), positionString.c_str());
		ImGui::PopFont();
	}
}
