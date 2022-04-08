#pragma once

#include "../../Sdk/Player/HitObject.h"
#include <algorithm>
#include <cmath>

class AimAssist
{
	static inline Vector2 windowSize;

	static inline Vector2 rawPosition;
	static inline Vector2 assistedPosition;
	
	static inline bool canAssist;
	static inline int currentIndex = 0;

	static inline int hitWindow50;
	static inline int preEmpt;
	static inline float hitObjectRadius;

	static inline HitObject currentHitObject;
	static inline HitObject previousHitObject;
	
	static inline Vector2 lastPos = Vector2(0, 0);
	static inline Vector2 lastPoint;
	static inline Vector2 lastTarget;
	static inline Vector2 sliderBallPos;
	
	static inline bool decided = true;

	static inline float distanceScaled;

	static __forceinline bool InCircle(Vector2 circle, float radius, Vector2 point);

	// This is the first version of the algorithm, it's pretty bad but it works!
	static Vector2 Algorithmv0(float strength, float distance, Vector2 hitObjectPosition, Vector2 cursorPosition, float strengthMultiplier);

	//kat's algo
	static inline Vector2 offset = Vector2();
public:
	static void DrawDebugOverlay();
	static void Initialize();

	static Vector2 DoAssist(Vector2 realPosition);
	static Vector2 DoAssistv2(Vector2 realPosition);
	static Vector2 DoAssistv3(Vector2 realPosition);
};