#pragma once
#include <vector>

class SpeedDifficultyEvaluator
{
	static inline constexpr int MIN_DELTA_TIME = 25;
	static inline constexpr double SINGLE_SPACING_THRESHOLD = 125.0;
	static inline constexpr double MIN_SPEED_BONUS = 75.0;
	static inline constexpr double SPEED_BALANCING_FACTOR = 40.0;
	static inline constexpr double DISTANCE_MULTIPLIER = 0.94;
	static inline constexpr double SPEED_SKILL_MULTIPLIER = 1375.0;
	static inline constexpr double STRAIN_DECAY_BASE = 0.3;

	static inline std::vector<double> difficulties;
	static inline std::vector<double> difficultiesNormalized;
	static inline std::vector<double> strainValues;
	static inline std::vector<double> strainValuesNormalized;

public:
	static void Initialize();

	static double DifficultyAt(int hitObjectIndex);
	static double NormalizedDifficultyAt(int hitObjectIndex);
	static double StrainAt(int hitObjectIndex);
	static double NormalizedStrainAt(int hitObjectIndex);
};
