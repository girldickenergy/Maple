#include "ApproachCircle.h"

using namespace ReplayEditor;

ApproachCircle::ApproachCircle(int* timer, Vector2 position, Transformation fade, Transformation scale, OsuDrawable* linkedObject, int index) :
	OsuDrawable(DrawableType::Drawable_ApproachCircle, timer, position, Transformation(), index)
{
	_linkedObject = linkedObject;

	PushTransformation(fade);
	PushTransformation(scale);
}

ImU32 ReplayEditor::ApproachCircle::GetHitColor()
{
	return _linkedObject->GetHitColor();
}
