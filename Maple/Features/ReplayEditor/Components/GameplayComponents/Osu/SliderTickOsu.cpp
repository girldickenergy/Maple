#include "SliderTickOsu.h"

ReplayEditor::SliderTickOsu::SliderTickOsu() : OsuDrawable(Drawable_HitObjectSliderBallOsu, nullptr, Vector2(0,0))
{ }

ReplayEditor::SliderTickOsu::SliderTickOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation)
	: OsuDrawable(DrawableType::Drawable_HitObjectSliderBallOsu, timer, position, transformation)
{
	_time = time;
}

void ReplayEditor::SliderTickOsu::SetTime(int time)
{
	_time = time;
}

int ReplayEditor::SliderTickOsu::GetTime()
{
	return _time;
}
