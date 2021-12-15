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
	if (Config::AimAssist::Enabled && Config::AimAssist::DrawDebugOverlay && Player::IsLoaded())
	{
		// Draw small debug box uwu
		ImGui::SetNextWindowSize(ImVec2(GameBase::GetClientBounds()->Width, GameBase::GetClientBounds()->Height));
		ImGui::Begin("aa", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			if (!assistedPosition.IsNull && canAssist)
			{
				std::string assistX = "X:" + std::to_string(assistedPosition.X);
				std::string assistY = "Y:" + std::to_string(assistedPosition.Y);
				drawList->AddRectFilled(ImVec2(assistedPosition.X + 30, assistedPosition.Y + 30), ImVec2(assistedPosition.X + 110, assistedPosition.Y + 70), ImGui::ColorConvertFloat4ToU32(ImVec4(227.f, 227.f, 227.f, 1.f)), 2.f);

				ImGui::PushFont(StyleProvider::FontSmallBold);
				ImGui::SetCursorPos(ImVec2(assistedPosition.X - 28, assistedPosition.Y - 28));
				ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), assistX.c_str());
				ImGui::SetCursorPos(ImVec2(assistedPosition.X - 28, assistedPosition.Y - 13));
				ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), assistY.c_str());
				ImGui::PopFont();

				// Draw circle on *actual* cursor position
				drawList->AddCircleFilled(ImVec2(rawPosition.X, rawPosition.Y), 12.f, ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 255.f, 0.f, 0.6f)));

				// Draw FOV
				drawList->AddCircleFilled(ImVec2(assistedPosition.X, assistedPosition.Y), distanceScaled, ImGui::ColorConvertFloat4ToU32(ImVec4(150.f, 219.f, 96.f, 0.4f)));

				// Draw Last position
				Vector2 screen = Config::AimAssist::Algorithm == 0 ? GameField::FieldToDisplay(lastPos) : lastPos;
				drawList->AddCircleFilled(ImVec2(screen.X, screen.Y), 20.f, ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 111.f, 70.f, 0.4f)));

				// Draw Sliderball position
				if (Config::AimAssist::Algorithm == 0)
				{
					Vector2 screen2 = GameField::FieldToDisplay(sliderBallPos);
					drawList->AddCircleFilled(ImVec2(screen2.X, screen2.Y), Config::AimAssist::SliderballDeadzone * 2, decided ? ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 255.f, 0.f, 0.5f)) : ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 0.f, 0.f, 0.5f)));
				}
			}
		}
		ImGui::End();
	}
}

Vector2 AimAssist::doAssist(Vector2 realPosition)
{
	if (!Config::AimAssist::Enabled || !Player::IsLoaded() || !canAssist)
		return realPosition;

	const float strength			  = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength / 2.f < 0.7f ? Config::AimAssist::EasyModeStrength / 2.f : (Config::AimAssist::EasyModeStrength / 2.f) - 0.214f : Config::AimAssist::Strength;
	const int baseFOV				  = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 50.f : Config::AimAssist::BaseFOV;
	const float maximumFOVScale		  = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 2.f + .25f : Config::AimAssist::MaximumFOVScale;
	const float minimumFOVTotal		  = Config::AimAssist::EasyMode ? 0 : Config::AimAssist::MinimumFOVTotal;
	const float maximumFOVTotal		  = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 220 : Config::AimAssist::MaximumFOVTotal;
	const bool assistOnSliders		  = Config::AimAssist::EasyMode ? true : Config::AimAssist::AssistOnSliders;
	const bool flipSliderballDeadzone = Config::AimAssist::EasyMode ? false : Config::AimAssist::FlipSliderballDeadzone;
	const float sliderballDeadzone	  = Config::AimAssist::EasyMode ? Config::AimAssist::EasyModeStrength * 12.2f + 2.1f : Config::AimAssist::SliderballDeadzone;
	const float strengthMultiplier	  = Config::AimAssist::EasyMode ? 1.f : Config::AimAssist::StrengthMultiplier;
	const float assistDeadzone		  = Config::AimAssist::EasyMode ? 3.f : Config::AimAssist::AssistDeadzone;
	const float resyncLeniency		  = Config::AimAssist::EasyMode ? 3.5f : Config::AimAssist::ResyncLeniency;
	const float resyncLeniencyFactor  = Config::AimAssist::EasyMode ? 0.693f : Config::AimAssist::ResyncLeniencyFactor;

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

static auto __forceinline lerp(float x, float y, float t) {
	_mm_store_ps(&t, _mm_min_ps(_mm_max_ps(_mm_load_ps(&t), _mm_set_ps1(0.f)), _mm_set_ps1(1.f)));

	const auto candidate = _mm_add_ps(_mm_load_ps(&x), _mm_mul_ps(_mm_sub_ps(_mm_load_ps(&y), _mm_load_ps(&x)), _mm_load_ps(&t)));

	if (!_mm_testz_ps(_mm_xor_ps(candidate, _mm_set_ps1(__builtin_huge_valf())), _mm_set_ps1(-0.f))) {
		return _mm_max_ps(_mm_set_ps1(x), _mm_set_ps1(y)).m128_f32[0];
	}

	return candidate.m128_f32[0];
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
	if (time > currentHitObject.StartTime)
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

	Vector2 hitObjectPosition = GameField::FieldToDisplay(currentHitObject.Position);

	Vector2 displacement = hitObjectPosition - realPosition;
	auto fov = (40.f * Config::AimAssist::Algorithmv2Power);

	if (point_in_radius(realPosition, hitObjectPosition, calc_fov_scale(time, currentHitObject.StartTime, hitWindow50, preEmpt) * fov)) {
		if (!point_in_radius(realPosition, lastPos, 1.75f) && Config::AimAssist::Algorithmv2Power && !previousHitObject.IsNull) {
			const auto interpolant = calc_interpolant(windowSize, displacement.Length(), Config::AimAssist::Algorithmv2Power);

			if (interpolant > std::numeric_limits<float>::epsilon()) {
				offset.X = std::clamp(lerp(offset.X, displacement.X, interpolant), -(Config::AimAssist::Algorithmv2Power * 16.f), Config::AimAssist::Algorithmv2Power * 16.f);
				offset.Y = std::clamp(lerp(offset.Y, displacement.Y, interpolant), -(Config::AimAssist::Algorithmv2Power * 16.f), Config::AimAssist::Algorithmv2Power * 16.f);
			}
		}
	}
	else if (offset.Length() > std::numeric_limits<float>::epsilon()) {
		offset = offset * Vector2(.95f, .95f); // could probably make this a little better, but it works perfectly lmao
	}

	lastPos = realPosition;
	assistedPosition = realPosition + offset;
	return assistedPosition;

}

void AimAssist::Reset()
{
	hitWindow50 = HitObjectManager::GetHitWindow50();
	preEmpt = HitObjectManager::GetPreEmpt();
	
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
	const Vector2 assistedPosition = Config::AimAssist::Algorithm == 0 ? doAssist(Vector2(x, y)) : doAssistv2(Vector2(x, y));

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
