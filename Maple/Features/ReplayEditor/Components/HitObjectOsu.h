#pragma once
#include "Drawable.h"

namespace ReplayEditor
{
	class HitObjectOsu : public Drawable
	{
		using Drawable::Drawable;
		
		int time;
		int preempt;
	public:
		HitObjectOsu(int _time, int _preempt, Vector2 _position, Transformation _transformation = Transformation());
		void SetTime(int _time);
		int GetTime();
		void SetPreempt(int _preempt);
		int GetPreempt();
	};
}