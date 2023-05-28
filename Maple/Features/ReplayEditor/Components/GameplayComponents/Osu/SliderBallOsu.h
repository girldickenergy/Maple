#pragma once
#include "OsuDrawable.h"

namespace ReplayEditor
{
	class SliderBallOsu : public OsuDrawable
	{
		using OsuDrawable::OsuDrawable;

	public:
		SliderBallOsu();
		SliderBallOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation = Transformation());
	};
}