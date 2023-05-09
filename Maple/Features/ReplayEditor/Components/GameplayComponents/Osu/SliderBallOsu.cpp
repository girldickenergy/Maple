#include "SliderBallOsu.h"

ReplayEditor::SliderBallOsu::SliderBallOsu() : Drawable(Drawable_HitObjectSliderBallOsu, nullptr, Vector2(0,0))
{ }

ReplayEditor::SliderBallOsu::SliderBallOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation)
	: Drawable(DrawableType::Drawable_HitObjectSliderBallOsu, timer, position, transformation)
{
	_time = time;
}

void ReplayEditor::SliderBallOsu::SetTime(int time)
{
	_time = time;
}

int ReplayEditor::SliderBallOsu::GetTime()
{
	return _time;
}
