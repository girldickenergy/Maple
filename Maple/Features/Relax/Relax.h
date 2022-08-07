#pragma once

#include <random>

#include "Osu/Keys/OsuKeys.h"
#include "../../SDK/Player/HitObject.h"

class Relax
{
	static inline int time;
	static inline double rateMultiplier;

	static inline int hitObjectsCount;
	static inline int currentIndex;
	static inline HitObject currentHitObject = HitObject();
	static inline HitObject lastHitObject = HitObject();
	static inline HitObject nextHitObject = HitObject();

	static inline std::default_random_engine rng = std::default_random_engine();
	static inline std::normal_distribution<double> normalDistribution = std::normal_distribution<double>(0, 1);
	static inline int hitWindow50;
	static inline int offset;
	static inline int holdTime;

	static inline float hitObjectRadius;
	static inline Vector2 cursorPosition;
	static inline bool wasInMissRadius;
	static inline Vector2 missPosition;

	static inline OsuKeys primaryKey;
	static inline OsuKeys secondaryKey;
	static inline OsuKeys currentKey;
	static inline bool primaryKeyPressed;
	static inline int primaryKeyPressTime;
	static inline int primaryKeyReleaseTime;
	static inline bool secondaryKeyPressed;
	static inline int secondaryKeyPressTime;
	static inline int secondaryKeyReleaseTime;

	static OsuKeys mapleKeyToOsuKey(int key);
	static void moveToNextHitObject();
	static void updateAlternation();
	static void updateTimings();
	static bool handleHitScan();
	static double calculateDirectionAngle(Vector2 lastPosition, Vector2 currentPosition, Vector2 hitObjectPosition);
	static void handleKeyPress();
	static void handleKeyRelease();
public:
	static inline bool IsRunning = false;

	static void Initialize();
	static OsuKeys Update();
};
