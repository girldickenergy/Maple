#pragma once
#include "OsuDrawable.h"

namespace ReplayEditor
{
	class ApproachCircle : public OsuDrawable
	{
		using OsuDrawable::OsuDrawable;

		OsuDrawable* _linkedObject;
	public:
		ApproachCircle(int* timer, Vector2 position, Transformation fade, Transformation scale, OsuDrawable* linkedObject, int index = 0);

		ImU32 GetHitColor();
	};
}