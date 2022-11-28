#define _USE_MATH_DEFINES
#include "Relax.h"

#include "../../Config/Config.h"
#include "../../Utilities/Security/Security.h"
#include "../../SDK/Mods/ModManager.h"
#include "../../SDK/Player/HitObjectManager.h"
#include "../../SDK/Input/InputManager.h"
#include "../../SDK/Audio/AudioEngine.h"
#include "../../SDK/Player/Player.h"
#include "../../SDK/Osu/GameField.h"

OsuKeys Relax::mapleKeyToOsuKey(int key)
{
	switch (key)
	{
		case 0:
			return OsuKeys::M1;
		case 1:
			return OsuKeys::K1;
		case 2:
			return OsuKeys::M2;
		case 3:
			return OsuKeys::K2;
	}

	return OsuKeys::None;
}

void Relax::moveToNextHitObject(int count)
{
	currentHitObjectIndex += count;
	if (currentHitObjectIndex < hitObjectsCount)
	{
		previousHitObject = currentHitObject;
		currentHitObject = nextHitObject;
		nextHitObject = currentHitObjectIndex + 1 < hitObjectsCount ? HitObjectManager::GetHitObject(currentHitObjectIndex + 1) : HitObject();

		wasInMissRadius = false;
		missPosition = Vector2();

		currentHitOffset = normalDistribution(rng) * (((-0.0007 - Config::Relax::Timing::TargetUnstableRate) / -3.9955) / 2.3);

		if (!previousHitObject.IsNull && 60000 / std::clamp(currentHitObject.StartTime - (previousHitObject.IsType(HitObjectType::Slider) && Config::Relax::SliderAlternationOverride ? previousHitObject.StartTime : previousHitObject.EndTime), 1, 60000) / 4 >= Config::Relax::AlternateBPM)
			currentKey = currentKey == primaryKey ? secondaryKey : primaryKey;
		else
			currentKey = primaryKey;
	}
}

HitScanResult Relax::handleHitScan()
{
	Vector2 lastCursorPosition = cursorPosition;
	cursorPosition = GameField::DisplayToField(InputManager::GetCursorPosition());

	if (time < currentHitObject.StartTime - allowableScanOffset)
		return HitScanResult::Wait;

	if (currentHitObject.IsType(HitObjectType::Spinner) && time >= currentHitObject.StartTime + currentHitOffset)
		return HitScanResult::Hit;

	const float distanceToObject = cursorPosition.Distance(currentHitObject.Position);
	const float lastDistanceToObject = lastCursorPosition.IsNull() ? 0 : lastCursorPosition.Distance(currentHitObject.Position);
	const float distanceToNext = nextHitObject.IsNull ? 0 : cursorPosition.Distance(nextHitObject.Position);
	const float distanceBetweenObjects = nextHitObject.IsNull ? 0 : currentHitObject.Position.Distance(nextHitObject.Position);

	if (time > currentHitObject.StartTime + allowableScanOffset)
	{
		if (distanceToObject <= hitObjectRadius * 3 && distanceToObject > hitObjectRadius)
			return HitScanResult::Hit;

		return HitScanResult::Skip;
	}

	const double directionAngleCurrent = calculateDirectionAngle(lastCursorPosition, cursorPosition, currentHitObject.Position);
	const double directionAngleNext = nextHitObject.IsNull ? 0 : calculateDirectionAngle(lastCursorPosition, cursorPosition, nextHitObject.Position);

	if (distanceToObject <= hitObjectRadius)
	{
		if (time >= currentHitObject.StartTime + currentHitOffset && distanceToObject <= hitObjectRadius * 0.9)
			return HitScanResult::Hit;

		if (currentHitObject.IsType(HitObjectType::Normal) && Config::Relax::HitScan::DirectionPredictionEnabled)
		{
			if (!lastCursorPosition.IsNull() && currentHitObjectIndex + 1 < hitObjectsCount && distanceToObject > lastDistanceToObject && distanceBetweenObjects > distanceToNext && distanceToObject > hitObjectRadius * Config::Relax::HitScan::DirectionPredictionScale)
			{
				if (directionAngleNext <= Config::Relax::HitScan::DirectionPredictionAngle)
					return HitScanResult::Hit;
			}
		}
	}

	if (currentHitObject.IsType(HitObjectType::Normal) && !nextHitObject.IsNull && distanceToObject > hitObjectRadius && distanceToNext > hitObjectRadius)
	{
		const float missRadius = hitObjectRadius + (distanceBetweenObjects - hitObjectRadius * 2) * (1.0f / 3.0f);

		if (!wasInMissRadius)
			wasInMissRadius = distanceToObject <= missRadius;

		if (!missPosition.IsNull() && directionAngleCurrent <= 35)
			missPosition = Vector2();
		else if (missPosition.IsNull() && directionAngleCurrent > 35 && directionAngleNext <= 35)
			missPosition = cursorPosition;

		if (directionAngleNext <= 35 && time > currentHitObject.StartTime + currentHitOffset && ((wasInMissRadius && distanceToObject > missRadius) || (!missPosition.IsNull() && missPosition.Distance(cursorPosition) > missRadius)))
			return HitScanResult::Hit;
	}

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

void Relax::handleKeyPress()
{
	const std::uniform_int_distribution<int> normalHoldTime = std::uniform_int_distribution<int>(Config::Relax::Timing::MinimumHoldTime, Config::Relax::Timing::MaximumHoldTime);
	const std::uniform_int_distribution<int> sliderHoldTime = std::uniform_int_distribution<int>(Config::Relax::Timing::MinimumSliderHoldTime, Config::Relax::Timing::MaximumSliderHoldTime);

	int releaseTime;
	if (Config::Relax::Blatant::UseLowestPossibleHoldTimes)
		releaseTime = (currentHitObject.IsType(HitObjectType::Normal) ? time : currentHitObject.EndTime) + (std::min)(25, nextHitObject.IsNull ? 25 : static_cast<int>((nextHitObject.StartTime - currentHitObject.EndTime) * 0.5f));
	else
		releaseTime = ((currentHitObject.IsType(HitObjectType::Normal) ? time + normalHoldTime(rng) : currentHitObject.EndTime) + sliderHoldTime(rng)) * rateMultiplier;

	if (currentKey == primaryKey)
	{
		primaryKeyPressed = true;
		primaryKeyPressTime = time;
		primaryKeyReleaseTime = releaseTime;
	}
	else
	{
		secondaryKeyPressed = true;
		secondaryKeyPressTime = time;
		secondaryKeyReleaseTime = releaseTime;
	}
}

void Relax::handleKeyRelease()
{
	if (primaryKeyPressed && primaryKeyReleaseTime <= time)
	{
		primaryKeyPressed = false;
		primaryKeyReleaseTime = 0;
	}

	if (secondaryKeyPressed && secondaryKeyReleaseTime <= time)
	{
		secondaryKeyPressed = false;
		secondaryKeyReleaseTime = 0;
	}
}

void Relax::Initialize()
{
	if (!Config::Relax::Enabled || Player::GetIsReplayMode() || Player::GetPlayMode() != PlayModes::Osu)
		return;

	//unstable rate approximation
	/*
	std::vector<double> urarr;
	for (int i = 0; i < 100; i++)
		urarr.push_back(0);
	int num = 25;
	for (int k = 0; k < 10000; k++)
	{
		if (k != 0 && k % 10 == 0)
			num += 25;

		for (int i = 0; i < 100; i++)
		{
			double ur = 0.0;
			int hits = 0;
			double totalHitValue = 0;
			for (int j = 0; j < num; j++)
			{
				hits++;
				auto hitValue = normalDistribution(rng) * (i / 2.5);
				totalHitValue += hitValue;
				auto avg = totalHitValue / hits;
				ur += std::pow(hitValue - avg, 2);
			}
			urarr[i] += std::sqrt(ur / hits) * 10;
		}
	}
	for (int i = 0; i < 100; i++)
		std::cout << urarr[i] / 10000.0 << std::endl;*/

	rateMultiplier = ModManager::GetModPlaybackRate() / 100.0;

	hitObjectsCount = HitObjectManager::GetHitObjectsCount();
	currentHitObjectIndex = HitObjectManager::GetCurrentHitObjectIndex();
	currentHitObject = HitObjectManager::GetHitObject(currentHitObjectIndex);
	nextHitObject = currentHitObjectIndex + 1 < hitObjectsCount ? HitObjectManager::GetHitObject(currentHitObjectIndex + 1) : HitObject();
	previousHitObject = currentHitObjectIndex - 1 >= 0 ? HitObjectManager::GetHitObject(currentHitObjectIndex - 1) : HitObject();

	hitWindow300 = HitObjectManager::GetHitWindow300();
	hitWindow100 = HitObjectManager::GetHitWindow100();
	hitWindow50 = HitObjectManager::GetHitWindow50();

	const int range = Config::Relax::Timing::AllowableHitRange;
	const int hitWindowStartTime = range <= 100 ? 0 : range <= 200 ? hitWindow300 + 1 : hitWindow100 + 1;
	const int hitWindowEndTime = range <= 100 ? hitWindow300 : range <= 200 ? hitWindow100 : hitWindow50;
	const int hitWindowTime = hitWindowEndTime - hitWindowStartTime;

	double multiplier = range / 100.0;
	if (multiplier != 0.0 && fmod(multiplier, 1.0) == 0)
		multiplier = 1;
	else
		multiplier = fmod(multiplier, 1.0);

	allowableScanOffset = hitWindowStartTime + (hitWindowTime * multiplier);

	currentHitOffset = normalDistribution(rng) * (((-0.0007 - Config::Relax::Timing::TargetUnstableRate) / -3.9955) / 2.3);

	hitObjectRadius = HitObjectManager::GetHitObjectRadius();
	cursorPosition = Vector2();
	wasInMissRadius = false;
	missPosition = Vector2();

	primaryKey = mapleKeyToOsuKey(Config::Relax::PrimaryKey);
	secondaryKey = mapleKeyToOsuKey(Config::Relax::SecondaryKey);
	currentKey = primaryKey;
	primaryKeyPressed = false;
	primaryKeyPressTime = INT_MIN;
	primaryKeyReleaseTime = 0;
	secondaryKeyPressed = false;
	secondaryKeyPressTime = INT_MIN;
	secondaryKeyReleaseTime = 0;

	IsRunning = true;
}

OsuKeys Relax::Update()
{
	time = AudioEngine::GetTime() - Config::Relax::Timing::Offset;

	if (currentHitObjectIndex < hitObjectsCount)
	{
		switch (handleHitScan())
		{
			case HitScanResult::Hit:
			{
				const bool fastSingletap = abs(time - (currentKey == primaryKey ? primaryKeyPressTime : secondaryKeyPressTime)) < 90 * rateMultiplier;
				if (!fastSingletap || Config::Relax::Blatant::UseLowestPossibleHoldTimes)
				{
					handleKeyPress();
					moveToNextHitObject();
				}
			}
				break;
			case HitScanResult::Skip:
				moveToNextHitObject();

				break;
			case HitScanResult::Wait:
				break;
		}
	}

	handleKeyRelease();

	return IsRunning ? (primaryKeyPressed && secondaryKeyPressed ? (primaryKey | secondaryKey) : primaryKeyPressed ? primaryKey : secondaryKeyPressed ? secondaryKey : OsuKeys::None) : OsuKeys::None;
}