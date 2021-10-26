#pragma once

#include "../../Sdk/Player/HitObject.h"
#include <algorithm>
#include <cmath>

class AimAssist
{
	static inline std::vector<HitObject> cachedHitObjects;
	static inline int noteIndex;
	static inline Vector2 lastPos = Vector2(0, 0);
	static inline Vector2 lastPoint;
	static inline Vector2 lastTarget;
	static inline HitObject lastObject;

	static inline Vector2 sliderBallPos;
	static inline bool decided = true;

	static inline float distanceScaled;

	static __forceinline bool InCircle(Vector2 circle, float radius, Vector2 point);

	// This is the first version of the algorithm, it's pretty bad but it works!
	static Vector2 Algorithmv0(float strength, float distance, Vector2 hitObjectPosition, Vector2 cursorPosition);

public:
	static void DrawDebugOverlay();

	static void AssistThread();
	static inline Vector2 RawPosition;
	static inline Vector2 AssistedPosition;

	static inline bool IsLoaded;
	static inline bool CanAssist;

	static Vector2 DoAssist(Vector2 realPosition);
	static void Load();
	static void Reset();

	typedef void(__stdcall* fnUpdateCursorPosition)(float x, float y);
	static inline fnUpdateCursorPosition oUpdateCursorPosition;
	static void __stdcall UpdateCursorPosition(float x, float y);
};