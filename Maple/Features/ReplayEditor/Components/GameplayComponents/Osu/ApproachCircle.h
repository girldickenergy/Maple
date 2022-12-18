#pragma once
#include "../Drawable.h"

namespace ReplayEditor
{
	class ApproachCircle : public Drawable
	{
		using Drawable::Drawable;

	public:
		ApproachCircle(int* timer, Vector2 position, Transformation fade, Transformation scale, int index = 0);
	};
}