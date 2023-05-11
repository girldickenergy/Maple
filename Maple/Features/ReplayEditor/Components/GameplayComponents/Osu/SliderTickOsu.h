#pragma once
#include "../Drawable.h"

namespace ReplayEditor
{
	class SliderTickOsu : public Drawable
	{
		using Drawable::Drawable;

		int _time;
	public:
		SliderTickOsu();
		SliderTickOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation = Transformation());
		void SetTime(int time);
		int GetTime();
	};
}