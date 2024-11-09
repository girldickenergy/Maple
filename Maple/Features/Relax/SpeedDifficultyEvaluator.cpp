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

		currentStrain *= std::pow(1.0 - STRAIN_DECAY_BASE, strainTime / 1000.0);

		strainTime /= std::clamp((strainTime / hitWindow300) / 0.93, 0.92, 1.0);

		double speedBonus = 0.0;

		if (strainTime < MIN_SPEED_BONUS)
			speedBonus = 0.75 * std::pow((MIN_SPEED_BONUS - strainTime) / SPEED_BALANCING_FACTOR, 2);

		double distance = SINGLE_SPACING_THRESHOLD;
		if (!previousHitObject.IsNull && !previousHitObject.IsType(HitObjectType::Spinner))
			distance = std::min(SINGLE_SPACING_THRESHOLD, static_cast<double>(previousHitObject.EndPosition.Distance(currentHitObject.Position)));

		double distanceBonus = std::pow(distance / SINGLE_SPACING_THRESHOLD, 3.95) * DISTANCE_MULTIPLIER;

		double difficulty = (1.0 + speedBonus + distanceBonus) * 1000.0 / strainTime;

		if (!currentHitObject.IsType(HitObjectType::Spinner))
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
