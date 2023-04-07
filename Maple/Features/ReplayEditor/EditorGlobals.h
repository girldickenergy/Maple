#pragma once
#include "Math/Vector2.h"

namespace ReplayEditor
{
	#define COL(x) ( x/255.f )
	#define PERC(x, y) ( (x * y) / 100.f )
	#define CIRCLESIZE(x) ( (64 * (1.0f - 0.7f * (x - 5) / 5) / 2) )

	class EditorGlobals
	{
	public:
		static inline Vector2 PlayfieldSize;
		static inline Vector2 PlayfieldOffset;

		static float PlayfieldScale(float y)
		{
			return y / (512.f / PlayfieldSize.X);
		}

		static Vector2 ConvertToPlayArea(Vector2 position)
		{
			float screenPosX = position.X / 512.f * PlayfieldSize.X + PlayfieldOffset.X;
			float screenPosY = position.Y / 384.f * PlayfieldSize.Y + PlayfieldOffset.Y;
			return { screenPosX, screenPosY };
		}

		static Vector2 DisplayToField(Vector2 position)
		{
			return (position - PlayfieldOffset) / (PlayfieldSize.Y / 384.f);
		}
	};
}
