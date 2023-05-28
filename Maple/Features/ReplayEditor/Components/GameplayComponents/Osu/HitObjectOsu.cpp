#include "HitObjectOsu.h"

ReplayEditor::HitObjectOsu::HitObjectOsu(int _time, int _preempt, int* _timer, Vector2 _position, Transformation _transformation, int index)
	: OsuDrawable(DrawableType::Drawable_HitObjectOsu, _timer, _time, _position, _transformation, index)
{
	preempt = _preempt;
}

void ReplayEditor::HitObjectOsu::SetPreempt(int _preempt)
{
	preempt = _preempt;
}

int ReplayEditor::HitObjectOsu::GetPreempt()
{
	return preempt;
}