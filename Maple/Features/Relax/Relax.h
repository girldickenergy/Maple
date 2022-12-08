#pragma once

#include <random>

#include "Osu/Keys/OsuKeys.h"
#include "../../SDK/Player/HitObject.h"

enum class HitScanResult
{
	Hit,
	Wait,
	Skip
};

class Relax
{
	static inline int time;
	static inline double rateMultiplier;

	static inline bool ignoreHitScan;

	static inline int hitObjectsCount;
	static inline int currentHitObjectIndex;
	static inline HitObject currentHitObject;
	static inline HitObject previousHitObject;
	static inline HitObject nextHitObject;

	static inline std::random_device rd{};
	static inline std::mt19937 gen{ rd() };
	static inline std::normal_distribution<double> normalDistribution = std::normal_distribution<double>(0, 1);
	static inline std::uniform_int_distribution<int> normalHoldTime;
	static inline std::uniform_int_distribution<int> sliderHoldTime;

	static inline int hitWindow300;
	static inline int hitWindow100;
	static inline int hitWindow50;
	static inline int allowableScanOffset;
	static inline int currentHitOffset;
	static inline int currentHoldTime;

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
	static void moveToNextHitObject(int skipCount = 0);
	static void updateTimings();
	static void updateAlternation();
	static HitScanResult handleHitScan();
	static double calculateDirectionAngle(Vector2 lastPosition, Vector2 currentPosition, Vector2 hitObjectPosition);
	static void handleKeyPress(int customEndTime = INT_MAX);
	static void handleKeyRelease();
public:
	static inline bool IsRunning = false;

	static void Initialize();
	static OsuKeys Update();
};
