#include "ApproachCircle.h"

using namespace ReplayEditor;

ApproachCircle::ApproachCircle(int* timer, Vector2 position, Transformation fade, Transformation scale, int index) :
	Drawable(DrawableType::Drawable_ApproachCircle, timer, position, Transformation(), index)
{
	PushTransformation(fade);
	PushTransformation(scale);
}