#pragma once
#include <vector>

class SpeedDifficultyEvaluator
{
	static inline constexpr int MIN_DELTA_TIME = 25;
	static inline constexpr double SINGLE_SPACING_THRESHOLD = 125.0;
	static inline constexpr double MIN_SPEED_BONUS = 75.0;
	static inline constexpr double SPEED_BALANCING_FACTOR = 40.0;
	static inline constexpr int NORMALIZED_RADIUS = 50;
	static inline constexpr double MAXIMUM_SLIDER_RADIUS = NORMALIZED_RADIUS * 2.4f;
	static inline constexpr double ASSUMED_SLIDER_RADIUS = NORMALIZED_RADIUS * 1.8f;
	static inline constexpr double SPEED_SKILL_MULTIPLIER = 1375.0;
	static inline constexpr double STRAIN_DECAY_BASE = 0.5;

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
