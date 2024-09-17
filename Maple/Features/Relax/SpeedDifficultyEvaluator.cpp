#include "SpeedDifficultyEvaluator.h"

#include <algorithm>
#include <numeric>

#include "../../SDK/Player/HitObjectManager.h"

void SpeedDifficultyEvaluator::Initialize()
{
	difficulties.clear();
	difficultiesNormalized.clear();
	strainValues.clear();
	strainValuesNormalized.clear();

	int hitObjectsCount = HitObjectManager::GetHitObjectsCount();
	int hitWindow300 = HitObjectManager::GetHitWindow300();
	float hitObjectRadius = HitObjectManager::GetHitObjectRadius();

	double currentStrain = 0.0;

	for (int i = 0; i < hitObjectsCount; i++)
	{
		auto currentHitObject = HitObjectManager::GetHitObject(i);
		auto previousHitObject = i - 1 >= 0 ? HitObjectManager::GetHitObject(i - 1) : HitObject();
		auto nextHitObject = i + 1 < hitObjectsCount ? HitObjectManager::GetHitObject(i + 1) : HitObject();

		double strainTime = std::max(previousHitObject.IsNull ? 0 : currentHitObject.StartTime - previousHitObject.StartTime, MIN_DELTA_TIME);
		currentStrain *= std::pow(1.0 - 0.5, strainTime / 1000.0 / 2.0);

		if (currentHitObject.IsType(HitObjectType::Spinner))
			strainValues.push_back(0);

		double doubletapness = 1;

		if (!nextHitObject.IsNull)
		{
			double currDeltaTime = std::max(1, previousHitObject.IsNull ? 0 : currentHitObject.StartTime - previousHitObject.StartTime);
			double nextDeltaTime = std::max(1, nextHitObject.StartTime - currentHitObject.StartTime);
			double deltaDifference = std::abs(nextDeltaTime - currDeltaTime);
			double speedRatio = currDeltaTime / std::max(currDeltaTime, deltaDifference);
			double windowRatio = std::pow(std::min(1.0, currDeltaTime / hitWindow300), 2);
			doubletapness = std::pow(speedRatio, 1.0 - windowRatio);
		}

		strainTime /= std::clamp((strainTime / hitWindow300) / 0.93, 0.92, 1.0);

		double speedBonus = 1.0;

		if (strainTime < MIN_SPEED_BONUS)
			speedBonus = 1 + 0.75 * std::pow((MIN_SPEED_BONUS - strainTime) / SPEED_BALANCING_FACTOR, 2);

		double distance = SINGLE_SPACING_THRESHOLD;
		if (!previousHitObject.IsNull && !previousHitObject.IsType(HitObjectType::Spinner))
		{
			float scalingFactor = NORMALIZED_RADIUS / hitObjectRadius;
			if (hitObjectRadius < 30.0)
				scalingFactor *= 1.0f + std::min(30.0f - hitObjectRadius, 5.0f) / 50.0f;

			double travelDistance = 0.0;
			if (currentHitObject.IsType(HitObjectType::Slider))
				travelDistance = currentHitObject.SpatialLength * currentHitObject.SegmentCount * std::pow(currentHitObject.SegmentCount / 2.5, 1.0 / 2.5);

			double minJumpDistance = (currentHitObject.Position * scalingFactor - previousHitObject.EndPosition * scalingFactor).Length();

			if (previousHitObject.IsType(HitObjectType::Slider))
			{
				double lastTravelTime = previousHitObject.Duration;
				double minJumpTime = std::max(strainTime - lastTravelTime, static_cast<double>(MIN_DELTA_TIME));
				float tailJumpDistance = (previousHitObject.EndPosition - currentHitObject.Position).Length() * scalingFactor;
				minJumpDistance = std::max(0.0, std::min(minJumpDistance - (MAXIMUM_SLIDER_RADIUS - ASSUMED_SLIDER_RADIUS), tailJumpDistance - MAXIMUM_SLIDER_RADIUS));
			}

			distance = std::min(SINGLE_SPACING_THRESHOLD, travelDistance + minJumpDistance);
		}

		double difficulty = (speedBonus + speedBonus * std::pow(distance / SINGLE_SPACING_THRESHOLD, 3.5)) * doubletapness / strainTime;

		currentStrain += difficulty * SPEED_SKILL_MULTIPLIER;

		difficulties.push_back(difficulty);
		strainValues.push_back(currentStrain);
	}

	double meanDifficulty = std::accumulate(difficulties.begin(), difficulties.end(), 0.0) / hitObjectsCount;
	double maxDifficulty = std::ranges::max(difficulties);
	for (const double difficulty : difficulties)
		difficultiesNormalized.push_back(std::clamp((difficulty / ((maxDifficulty + meanDifficulty) / 2.0)), 0.0, 1.0));

	double meanStrain = std::accumulate(strainValues.begin(), strainValues.end(), 0.0) / hitObjectsCount;
	double maxStrain = std::ranges::max(strainValues);
	for (const double strainValue : strainValues)
		strainValuesNormalized.push_back(std::clamp((strainValue / ((maxStrain + meanStrain) / 2.0)), 0.0, 1.0));
}

double SpeedDifficultyEvaluator::DifficultyAt(int hitObjectIndex)
{
	if (hitObjectIndex > 0 && hitObjectIndex < difficulties.size())
		return difficulties[hitObjectIndex];

	return 0.0;
}

double SpeedDifficultyEvaluator::NormalizedDifficultyAt(int hitObjectIndex)
{
	if (hitObjectIndex > 0 && hitObjectIndex < difficultiesNormalized.size())
		return difficultiesNormalized[hitObjectIndex];

	return 0.0;
}

double SpeedDifficultyEvaluator::StrainAt(int hitObjectIndex)
{
	if (hitObjectIndex > 0 && hitObjectIndex < strainValues.size())
		return strainValues[hitObjectIndex];

	return 0.0;
}

double SpeedDifficultyEvaluator::NormalizedStrainAt(int hitObjectIndex)
{
	if (hitObjectIndex > 0 && hitObjectIndex < strainValuesNormalized.size())
		return strainValuesNormalized[hitObjectIndex];

	return 0.0;
}
