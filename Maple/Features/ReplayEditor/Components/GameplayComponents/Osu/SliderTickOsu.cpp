#include "SliderTickOsu.h"

ReplayEditor::SliderTickOsu::SliderTickOsu() : OsuDrawable(Drawable_HitObjectSliderBallOsu, nullptr, 0, Vector2(0,0))
{ }

ReplayEditor::SliderTickOsu::SliderTickOsu(int time, int preempt, int* timer, Vector2 position, Transformation transformation)
	: OsuDrawable(DrawableType::Drawable_HitObjectSliderBallOsu, timer, time, position, transformation)
{ }
