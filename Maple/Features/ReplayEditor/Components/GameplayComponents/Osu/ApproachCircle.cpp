#include "ApproachCircle.h"

using namespace ReplayEditor;

ApproachCircle::ApproachCircle(int* timer, Vector2 position, Transformation fade, Transformation scale, OsuDrawable* linkedObject) :
	OsuDrawable(DrawableType::Drawable_ApproachCircle, timer, position, Transformation())
{
	_linkedObject = linkedObject;

	PushTransformation(fade);
	PushTransformation(scale);
}

void ReplayEditor::ApproachCircle::SetLinkedObject(OsuDrawable* linkedObject)
{
	_linkedObject = linkedObject;
}

OsuDrawable* ReplayEditor::ApproachCircle::GetLinkedObject()
{
	return _linkedObject;
}

ImU32 ReplayEditor::ApproachCircle::GetHitColor()
{
	return _linkedObject->GetHitColor();
}
