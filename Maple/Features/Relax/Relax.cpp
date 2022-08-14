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

void Relax::moveToNextHitObject()
{
	currentIndex++;
	if (currentIndex >= hitObjectsCount)
		return;

	lastHitObject = currentHitObject;
	currentHitObject = nextHitObject;
	nextHitObject = currentIndex + 1 < hitObjectsCount ? HitObjectManager::GetHitObject(currentIndex + 1) : HitObject();

	wasInMissRadius = false;
	missPosition = Vector2();

	updateTimings();
	updateAlternation();
}

void Relax::updateAlternation()
{
	if (!lastHitObject.IsNull && 60000 / std::clamp(currentHitObject.StartTime - (lastHitObject.IsType(HitObjectType::Slider) && Config::Relax::SliderAlternationOverride ? lastHitObject.StartTime : lastHitObject.EndTime), 1, 60000) / 4 >= Config::Relax::AlternateBPM)
		currentKey = currentKey == primaryKey ? secondaryKey : primaryKey;
	else
		currentKey = primaryKey;
}

void Relax::updateTimings()
{
	//approximation is made using 2.5 sigma, we're using 2.3 sigma here to account for the distribution error
	offset = normalDistribution(rng) * (((Config::Relax::Timing::TargetUnstableRate - 0.00533158) / 3.99535) / 2.3);
	holdTime = Config::Relax::Blatant::UseLowestPossibleHoldTimes ? (std::min)(25, nextHitObject.IsNull ? 25 : (int)((nextHitObject.StartTime - currentHitObject.EndTime) * 0.5f)) : ((currentHitObject.IsType(HitObjectType::Normal) ? Config::Relax::Timing::AverageHoldTime : Config::Relax::Timing::AverageSliderHoldTime) + (normalDistribution(rng) * ((currentHitObject.IsType(HitObjectType::Normal) ? Config::Relax::Timing::AverageHoldTimeError : Config::Relax::Timing::AverageSliderHoldTimeError) / 2.5))) * rateMultiplier;
}

bool Relax::handleHitScan()
{
	Vector2 lastCursorPosition = cursorPosition;
	cursorPosition = GameField::DisplayToField(InputManager::GetCursorPosition());

	if (currentHitObject.IsType(HitObjectType::Spinner) && time >= currentHitObject.StartTime + offset)
		return true;

	if (!Config::Relax::HitScan::WaitLateEnabled && time >= currentHitObject.StartTime + (((Config::Relax::Timing::TargetUnstableRate - 0.00533158) / 3.99535) / 2.3))
		return true;

	const float distanceToObject = cursorPosition.Distance(currentHitObject.Position);
	const float lastDistanceToObject = lastCursorPosition.IsNull() ? 0 : lastCursorPosition.Distance(currentHitObject.Position);
	const float distanceToNext = nextHitObject.IsNull ? 0 : cursorPosition.Distance(nextHitObject.Position);
	const float distanceBetweenObjects = nextHitObject.IsNull ? 0 : currentHitObject.Position.Distance(nextHitObject.Position);

	const double directionAngleCurrent = calculateDirectionAngle(lastCursorPosition, cursorPosition, currentHitObject.Position);
	const double directionAngleNext = nextHitObject.IsNull ? 0 : calculateDirectionAngle(lastCursorPosition, cursorPosition, nextHitObject.Position);

	if (distanceToObject <= hitObjectRadius)
	{
		if (time >= currentHitObject.StartTime + offset && distanceToObject <= hitObjectRadius * 0.9)
			return true;

		if (currentHitObject.IsType(HitObjectType::Normal) && Config::Relax::HitScan::DirectionPredictionEnabled)
		{
			if (!lastCursorPosition.IsNull() && currentIndex + 1 < hitObjectsCount && distanceToObject > lastDistanceToObject && distanceBetweenObjects > distanceToNext && distanceToObject > hitObjectRadius * Config::Relax::HitScan::DirectionPredictionScale)
			{
				if (directionAngleNext <= Config::Relax::HitScan::DirectionPredictionAngle)
					return true;
			}
		}
	}

	if (currentHitObject.IsType(HitObjectType::Normal) && !nextHitObject.IsNull && distanceToObject > hitObjectRadius && distanceToNext > hitObjectRadius)
	{
		float missRadius = hitObjectRadius + (distanceBetweenObjects - hitObjectRadius * 2) * (1.0f / 3.0f);

		if (!wasInMissRadius)
			wasInMissRadius = distanceToObject <= missRadius;

		if (!missPosition.IsNull() && directionAngleCurrent <= 35)
			missPosition = Vector2();
		else if (missPosition.IsNull() && directionAngleCurrent > 35 && directionAngleNext <= 35)
			missPosition = cursorPosition;

		if (directionAngleNext <= 35 && time > currentHitObject.StartTime + offset && ((wasInMissRadius && distanceToObject > missRadius) || (!missPosition.IsNull() && missPosition.Distance(cursorPosition) > missRadius)))
			return true;
	}

	return false;
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
	if (currentKey == primaryKey)
	{
		primaryKeyPressed = true;
		primaryKeyPressTime = time;
		primaryKeyReleaseTime = (currentHitObject.IsType(HitObjectType::Normal) ? time : currentHitObject.EndTime) + holdTime;
	}
	else
	{
		secondaryKeyPressed = true;
		secondaryKeyPressTime = time;
		secondaryKeyReleaseTime = (currentHitObject.IsType(HitObjectType::Normal) ? time : currentHitObject.EndTime) + holdTime;
	}
}

void Relax::handleKeyRelease()
{
	if (primaryKeyPressed && primaryKeyReleaseTime <= AudioEngine::GetTime())
	{
		primaryKeyPressed = false;
		primaryKeyReleaseTime = 0;
	}

	if (secondaryKeyPressed && secondaryKeyReleaseTime <= AudioEngine::GetTime())
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
	currentIndex = HitObjectManager::GetCurrentHitObjectIndex();
	currentHitObject = HitObjectManager::GetHitObject(currentIndex);
	nextHitObject = currentIndex + 1 < hitObjectsCount ? HitObjectManager::GetHitObject(currentIndex + 1) : HitObject();
	lastHitObject = currentIndex - 1 >= 0 ? HitObjectManager::GetHitObject(currentIndex - 1) : HitObject();

	hitWindow50 = HitObjectManager::GetHitWindow50();
	updateTimings();

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

	if (currentIndex < hitObjectsCount && time >= currentHitObject.StartTime - hitWindow50)
	{
		bool fastSingletap = abs(time - (currentKey == primaryKey ? primaryKeyPressTime : secondaryKeyPressTime)) < 90 * rateMultiplier;
		if ((handleHitScan() && (!fastSingletap || Config::Relax::Blatant::UseLowestPossibleHoldTimes)) || time > currentHitObject.StartTime + hitWindow50)
		{
			handleKeyPress();
			moveToNextHitObject();
		}
	}

	handleKeyRelease();

	return IsRunning ? (primaryKeyPressed && secondaryKeyPressed ? (primaryKey | secondaryKey) : primaryKeyPressed ? primaryKey : secondaryKeyPressed ? secondaryKey : OsuKeys::None) : OsuKeys::None;
}