#include "HitObjectOsu.h"

ReplayEditor::HitObjectOsu::HitObjectOsu(int _time, int _preempt, int* _timer, Vector2 _position, Transformation _transformation)
	: Drawable(DrawableType::Drawable_HitObjectOsu, _timer, _position, _transformation)
{
	time = _time;
	preempt = _preempt;
}

void ReplayEditor::HitObjectOsu::SetTime(int _time)
{
	time = _time;
}

int ReplayEditor::HitObjectOsu::GetTime()
{
	return time;
}

void ReplayEditor::HitObjectOsu::SetPreempt(int _preempt)
{
	preempt = _preempt;
}

int ReplayEditor::HitObjectOsu::GetPreempt()
{
	return preempt;
}