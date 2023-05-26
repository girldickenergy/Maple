#pragma once
#include "OsuDrawable.h"

namespace ReplayEditor
{
	class SliderTickOsu : public OsuDrawable
	{
		using OsuDrawable::OsuDrawable;

		int _time;
	public:
		SliderTickOsu();
		SliderTickOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation = Transformation());
		void SetTime(int time);
		int GetTime();
	};
}