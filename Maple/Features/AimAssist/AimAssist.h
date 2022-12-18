#pragma once

#include "../../SDK/Player/HitObject.h"

class AimAssist
{
	static inline Vector2 windowSize;

	static inline int currentIndex = 0;

	static inline int hitWindow50;
	static inline int preEmpt;
	static inline float hitObjectRadius;

	static inline float fov;

	static inline HitObject currentHitObject;
	static inline HitObject previousHitObject;

	static Vector2 algorithmv1(Vector2 pos);
	static Vector2 algorithmv2(Vector2 pos);
	static Vector2 algorithmv3(Vector2 pos);
public:
	static void Initialize();

	static Vector2 GetCursorPosition(Vector2 pos);
	static void Render();
};