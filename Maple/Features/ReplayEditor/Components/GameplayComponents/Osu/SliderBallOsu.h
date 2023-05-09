#pragma once
#include "../Drawable.h"

namespace ReplayEditor
{
	class SliderBallOsu : public Drawable
	{
		using Drawable::Drawable;

		int _time;
	public:
		SliderBallOsu();
		SliderBallOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation = Transformation());
		void SetTime(int time);
		int GetTime();
	};
}