#pragma once

#include <windows.h>
#include <random>

#include "HitScanResult.h"
#include "HitTimings.h"
#include "../../Sdk/DataTypes/Enums/PlayKeys.h"
#include "../../Sdk/Player/HitObject.h"

class Relax
{
#pragma region Internals
	static inline bool isWorking = false;
	static inline bool shouldStop = false;
	static void relaxThread();
#pragma endregion

#pragma region Relax
	//audio stuff
	static inline int time;
	static inline double rateMultiplier;
	//audio stuff

	//hit object stuff
	static inline int hitObjectsCount;
	static inline int currentIndex;
	static inline HitObject currentHitObject = HitObject();
	static inline HitObject lastHitObject = HitObject();
	static inline HitObject nextHitObject = HitObject();

	static void reset();
	static void moveToNextHitObject(bool updateKeys);
	//hit object stuff

	//hit distribution
	static inline std::default_random_engine rng = std::default_random_engine();
	static inline std::uniform_real_distribution<> uniformDistribution = std::uniform_real_distribution<>(-1.0, 1.0);
	static inline std::normal_distribution<double> normalDistribution = std::normal_distribution<double>(0, 1);
	static inline std::uniform_int_distribution<int> randomHoldTime = std::uniform_int_distribution<int>(30, 50);

	static inline int hitWindow300;
	static inline int hitWindow100;
	static inline int hitWindow50;
	static inline HitTimings hitTimings;

	static HitTimings calculateTimings(bool alternating);
	//hit distribution

	//input
	static inline int primaryKey;
	static inline int secondaryKey;
	static inline int currentKeyCode;
	static inline PlayKeys currentKey;
	static inline bool isKeyDown;

	static bool updateAlternation();
	//input
#pragma endregion 

#pragma region HitScan
	static inline float hitObjectRadius;
	static inline bool wasInMissRadius;
	static inline Vector2 missPosition;
	static inline Vector2 cursorPosition;

	static HitScanResult getHitScanResult();
	static HitScanResult getNormalHitScanResult();
	static HitScanResult getSliderHitScanResult();
	static HitScanResult getSpinnerHitScanResult();
	static double calculateDirectionAngle(Vector2 lastPosition, Vector2 currentPosition, Vector2 hitObjectPosition);
#pragma endregion
public:
	static void Start();
	static void Stop();

	typedef int(__fastcall* fnUpdateKeyboardInput)(uintptr_t instance, int key);
	static inline fnUpdateKeyboardInput oUpdateKeyboardInput;
	static int __fastcall UpdateKeyboardInput(uintptr_t instance, int key);
};