#define _USE_MATH_DEFINES
#include "Relax.h"

#include <ThemidaSDK.h>
#include <cmath>

#include "../../Communication/Communication.h"
#include "../../Config/Config.h"
#include "../../Dependencies/InputSimulator/InputSimulator.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Bindings/BindingManager.h"
#include "../../Sdk/Input/InputManager.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Player/Player.h"
#include "../../Utilities/Security/Security.h"
#include "../../Utilities/Timing/WaitableTimer.h"

int i = 0;
void Relax::relaxThread()
{
	isWorking = true;
	shouldStop = false;
	
	while (!Player::IsLoaded())
	{
		if (shouldStop)
		{
			isWorking = false;
			
			return;
		}
		
		Sleep(1);
	}

	reset();

	int hitTime = 0;
	while (Config::Relax::Enabled && Player::IsLoaded() && currentIndex < hitObjectsCount && !shouldStop)
	{
		if (isKeyDown)
		{
			VM_SHARK_BLACK_START

			if (i == 0)
			{
				DWORD check1 = 0x2F47C114;
				CHECK_CODE_INTEGRITY(check1, 0xC0CEA1FA);
				if (check1 == 0x2F47C114)
					Security::CorruptMemory();

				Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle, true);
			}

			i++;
			if (i == 60000)
				i = 0;

			VM_SHARK_BLACK_END
		}
		
		WaitableTimer::Sleep(5000); //500 microseconds
		
		time = AudioEngine::TimeAccurate();
		if (time < currentHitObject.StartTime - hitWindow50 || !Player::IsPlaying())
			continue;

		if (!isKeyDown)
		{
			const auto hitScanResult = getHitScanResult();
			switch (hitScanResult)
			{
			case HitScanResult::Hit:
			{
				hitTime = time;
				isKeyDown = true;

				if (currentKey == PlayKeys::M1) //todo: handle mouse input through osu's input manager
					InputSimulator::LeftMouseButtonDown();
				else if (currentKey == PlayKeys::M2)
					InputSimulator::RightMouseButtonDown();

				break;
			}
			case HitScanResult::Skip:
				moveToNextHitObject(true);
				break;
			default:
				break;
			}
		}
		else if (time >= (currentHitObject.IsType(HitObjectType::Normal) ? hitTime : currentHitObject.EndTime) + hitTimings.HoldTime)
		{
			bool keepHolding = Config::Relax::HoldConsecutiveSpinners && currentIndex + 1 < hitObjectsCount && nextHitObject.IsType(HitObjectType::Spinner) && nextHitObject.StartTime - currentHitObject.EndTime <= 500 / rateMultiplier;

			if (!keepHolding)
			{
				isKeyDown = false;

				if (currentKey == PlayKeys::M1)
					InputSimulator::LeftMouseButtonUp();
				else if (currentKey == PlayKeys::M2)
					InputSimulator::RightMouseButtonUp();
			}

			moveToNextHitObject(!keepHolding);
		}
	}

	isKeyDown = false;

	isWorking = false;
}

void Relax::reset()
{
	primaryKey = BindingManager::GetPlayKey(PlayKeys::K1);
	secondaryKey = BindingManager::GetPlayKey(PlayKeys::K2);
	currentKey = static_cast<PlayKeys>(Config::Relax::PrimaryKey);
	isKeyDown = false;

	hitObjectsCount = HitObjectManager::GetHitObjectsCount();
	currentIndex = HitObjectManager::GetCurrentHitObjectIndex();
	currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	lastHitObject = currentIndex == 0 ? HitObject() : HitObjectManager::GetHitObject(currentIndex - 1);
	nextHitObject = currentIndex >= hitObjectsCount ? HitObject() : HitObjectManager::GetHitObject(currentIndex + 1);
	hitWindow300 = HitObjectManager::GetHitWindow300();
	hitWindow100 = HitObjectManager::GetHitWindow100();
	hitWindow50 = HitObjectManager::GetHitWindow50();
	hitTimings = calculateTimings(updateAlternation());

	hitObjectRadius = HitObjectManager::GetHitObjectRadius();
	cursorPosition = Vector2();
	wasInMissRadius = false;
	missPosition = Vector2();

	rateMultiplier = ModManager::ModPlaybackRate() / 100.0;
}

void Relax::moveToNextHitObject(bool updateKeys)
{
	currentIndex++;
	if (currentIndex >= hitObjectsCount)
		return;

	lastHitObject = currentHitObject;
	currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	nextHitObject = currentIndex + 1 >= hitObjectsCount ? HitObject() : HitObjectManager::GetHitObject(currentIndex + 1);
	wasInMissRadius = false;
	missPosition = Vector2();

	if (updateKeys)
		hitTimings = calculateTimings(updateAlternation());
}

HitTimings Relax::calculateTimings(bool alternating)
{
	const int spread = alternating ? Config::Relax::AlternationHitSpread : Config::Relax::HitSpread;
	const int hitWindowStartTime = spread <= 100 ? 0 : spread <= 200 ? hitWindow300 + 1 : hitWindow100 + 1;
	const int hitWindowEndTime = spread <= 100 ? hitWindow300 : spread <= 200 ? hitWindow100 : hitWindow50;
	const int hitWindowTime = hitWindowEndTime - hitWindowStartTime;

	double multiplier = spread / 100.0;
	if (multiplier != 0.0 && fmod(multiplier, 1.0) == 0)
		multiplier = 1;
	else
		multiplier = fmod(multiplier, 1.0);

	const double window = hitWindowStartTime + (hitWindowTime * multiplier);

	double offset;
	if (Config::Relax::Distribution == 1)
	{
		offset = normalDistribution(rng) * (window / 2);
		if (offset < -window || offset > window)
			offset = normalDistribution(rng) * (window / 3);
	}
	else offset = uniformDistribution(rng) * window;

	double holdTime = randomHoldTime(rng);
	if (holdTime > 0)
	{
		holdTime *= rateMultiplier;

		if (!nextHitObject.IsNull)
		{
			int diff = nextHitObject.StartTime - currentHitObject.EndTime;
			if (holdTime > diff)
				holdTime = diff;
		}
	}
	return HitTimings(static_cast<int>(offset), static_cast<int>(holdTime));
}

bool Relax::updateAlternation()
{
	bool alternateCurrentNote = false;
	bool alternateNextNote = false;

	int bpm = Config::Relax::MaxSingletapBPM / rateMultiplier;

	int diff1 = std::clamp(currentHitObject.StartTime - (lastHitObject.IsType(HitObjectType::Slider) && Config::Relax::SliderAlternationOverride ? lastHitObject.StartTime : lastHitObject.EndTime), 1, 60000);
	if (currentIndex > 0 && 60000 / diff1 / 4 > bpm || Config::Relax::Playstyle == 1)
		alternateCurrentNote = true;

	int diff2 = std::clamp(nextHitObject.StartTime - (currentHitObject.IsType(HitObjectType::Slider) && Config::Relax::SliderAlternationOverride ? currentHitObject.StartTime : currentHitObject.EndTime), 1, 60000);
	if (currentIndex + 1 < hitObjectsCount && 60000 / diff2 / 4 > bpm)
		alternateNextNote = true;

	if (Config::Relax::Playstyle == 1 && currentIndex == 0)
	{
		alternateCurrentNote = false;
		alternateNextNote = false;
	}

	if (!alternateCurrentNote && !alternateNextNote)
		currentKey = Config::Relax::Playstyle == 2 ? PlayKeys::M1 : (PlayKeys)Config::Relax::PrimaryKey;

	if (alternateCurrentNote)
		currentKey = currentKey == (PlayKeys)Config::Relax::PrimaryKey ? (PlayKeys)Config::Relax::SecondaryKey : (PlayKeys)Config::Relax::PrimaryKey;
	else if (alternateNextNote && Config::Relax::Playstyle == 2)
		currentKey = (PlayKeys)Config::Relax::PrimaryKey;

	currentKeyCode = currentKey == PlayKeys::K2 ? secondaryKey : primaryKey;
	return alternateCurrentNote;
}

HitScanResult Relax::getHitScanResult()
{
	if (currentHitObject.IsType(HitObjectType::Normal))
		return getNormalHitScanResult();

	if (currentHitObject.IsType(HitObjectType::Slider))
		return getSliderHitScanResult();

	return getSpinnerHitScanResult();
}

HitScanResult Relax::getNormalHitScanResult()
{
	Vector2 lastCursorPosition = cursorPosition;
	cursorPosition = InputManager::CursorPosition();
	
	const float distanceToObject = cursorPosition.Distance(currentHitObject.Position);
	const float lastDistanceToObject = lastCursorPosition.IsNull ? 0 : lastCursorPosition.Distance(currentHitObject.Position);
	const float distanceToNext = nextHitObject.IsNull ? 0 : cursorPosition.Distance(nextHitObject.Position);
	const float distanceBetweenObjects = nextHitObject.IsNull ? 0 : currentHitObject.Position.Distance(nextHitObject.Position);

	const double directionAngleCurrent = calculateDirectionAngle(lastCursorPosition, cursorPosition, currentHitObject.Position);
	const double directionAngleNext = nextHitObject.IsNull ? 0 : calculateDirectionAngle(lastCursorPosition, cursorPosition, nextHitObject.Position);

	if (distanceToObject <= hitObjectRadius)
	{
		if (time >= currentHitObject.StartTime + hitTimings.Offset)
			return HitScanResult::Hit;

		if (Config::Relax::PredictionEnabled)
		{
			if (!lastCursorPosition.IsNull && currentIndex + 1 < hitObjectsCount && distanceToObject > lastDistanceToObject && distanceBetweenObjects > distanceToNext && distanceToObject > hitObjectRadius * Config::Relax::PredictionScale)
			{
				if (directionAngleNext <= Config::Relax::PredictionAngle)
					return HitScanResult::Hit;
			}
		}
	}

	if (time > currentHitObject.StartTime + hitWindow50)
		return (nextHitObject.IsNull ? true : distanceToNext > hitObjectRadius) ? HitScanResult::Hit : HitScanResult::Skip;

	if (!nextHitObject.IsNull && distanceToObject > hitObjectRadius && distanceToNext > hitObjectRadius)
	{
		float missRadius = hitObjectRadius + (distanceBetweenObjects - hitObjectRadius * 2) * (1.0f / 3.0f);

		if (!wasInMissRadius)
			wasInMissRadius = distanceToObject <= missRadius;

		if (!missPosition.IsNull && directionAngleCurrent <= 35)
			missPosition = Vector2();
		else if (missPosition.IsNull && directionAngleCurrent > 35 && directionAngleNext <= 35)
			missPosition = cursorPosition;

		if (directionAngleNext <= 35 && time > currentHitObject.StartTime + hitTimings.Offset && ((wasInMissRadius && distanceToObject > missRadius) || (!missPosition.IsNull && missPosition.Distance(cursorPosition) > missRadius)))
			return HitScanResult::Hit;
	}
	return HitScanResult::Wait;
}

HitScanResult Relax::getSliderHitScanResult()
{
	Vector2 lastCursorPosition = cursorPosition;
	cursorPosition = InputManager::CursorPosition();

	const float distanceToObject = cursorPosition.Distance(currentHitObject.Position);

	if (distanceToObject <= hitObjectRadius)
	{
		if (time >= currentHitObject.StartTime + hitTimings.Offset)
			return HitScanResult::Hit;

		if (Config::Relax::PredictionEnabled && Config::Relax::SliderPredictionEnabled)
		{
			const Vector2 sliderBodyPosition = currentHitObject.PositionAtTime(currentHitObject.StartTime + hitWindow50 + 10);

			const float distanceFromObjectToSliderBody = currentHitObject.Position.Distance(sliderBodyPosition);
			const float distanceToSliderBody = cursorPosition.Distance(sliderBodyPosition);
			const float lastDistanceToSliderBody = lastCursorPosition.IsNull ? 0 : lastCursorPosition.Distance(sliderBodyPosition);

			const double directionAngle = calculateDirectionAngle(lastCursorPosition, cursorPosition, sliderBodyPosition);

			if (!lastCursorPosition.IsNull && distanceToSliderBody < lastDistanceToSliderBody && distanceToSliderBody < distanceFromObjectToSliderBody && distanceToObject > hitObjectRadius * Config::Relax::PredictionScale)
			{
				if (directionAngle <= Config::Relax::PredictionAngle)
					return HitScanResult::Hit;
			}
		}
	}

	if (time > currentHitObject.StartTime + hitWindow50)
		return HitScanResult::Hit;
	return HitScanResult::Wait;
}

HitScanResult Relax::getSpinnerHitScanResult()
{
	if (time >= currentHitObject.StartTime + hitTimings.Offset)
		return HitScanResult::Hit;

	return HitScanResult::Wait;
}

double Relax::calculateDirectionAngle(Vector2 lastPosition, Vector2 currentPosition, Vector2 hitObjectPosition)
{
	Vector2 v1 = lastPosition - currentPosition;
	Vector2 v2 = lastPosition - hitObjectPosition;

	const float denominator = sqrtf(v1.LengthSquared() * v2.LengthSquared());
	if (denominator < 1e-15f)
		return 0;

	float dot = v1.Dot(v2) / denominator;
	if (dot < -1)
		dot = -1;
	if (dot > 1)
		dot = 1;

	const double angle = acosf(dot);

	return angle * (360.0 / (M_PI * 2.0));
}

void Relax::Start()
{
	if (!Config::Relax::Enabled || Player::IsReplayMode() || Player::PlayMode() != PlayModes::Osu)
		return;

	Stop();

	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(relaxThread), nullptr, 0, nullptr);
}

void Relax::Stop()
{
	if (isWorking)
	{
		shouldStop = true;
		while (isWorking)
			Sleep(1);
	}
}

int __fastcall Relax::UpdateKeyboardInput(uintptr_t instance, int key)
{
	if (Config::Relax::Enabled && isKeyDown && ((currentKey == PlayKeys::K1 || currentKey == PlayKeys::K2) && key == currentKeyCode))
		return 1;

	return oUpdateKeyboardInput(instance, key);
}