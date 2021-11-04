#include "ApproachCircle.h"

using namespace ReplayEditor;

ApproachCircle::ApproachCircle(Drawable* pairedDrawable, Transformation _fade, Transformation _scale) :
	Drawable(DrawableType::Drawable_ApproachCircle, pairedDrawable->GetTimer(), pairedDrawable->GetPosition())
{
	PushTransformation(_fade);
	PushTransformation(_scale);
}