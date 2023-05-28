#include "SliderBallOsu.h"

ReplayEditor::SliderBallOsu::SliderBallOsu() : OsuDrawable(Drawable_HitObjectSliderBallOsu, nullptr, 0, Vector2(0,0))
{ }

ReplayEditor::SliderBallOsu::SliderBallOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation)
	: OsuDrawable(DrawableType::Drawable_HitObjectSliderBallOsu, timer, time, position, transformation)
{
}