#pragma once
#include "OsuDrawable.h"

namespace ReplayEditor
{
	class HitObjectOsu : public OsuDrawable
	{
		using OsuDrawable::OsuDrawable;
		
		int time;
		int preempt;
	public:
		HitObjectOsu(int _time, int _preempt, int* _timer, Vector2 _position, Transformation _transformation = Transformation(), int index = 0);
		void SetTime(int _time);
		int GetTime();
		void SetPreempt(int _preempt);
		int GetPreempt();
	};
}