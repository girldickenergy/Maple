#include "Drawable.h"

using namespace ReplayEditor;

Drawable::Drawable(DrawableType _drawableType, int* _timer, Vector2 _position, Transformation _transformation = Transformation())
{
	drawableType = _drawableType;
	timer = _timer;
	position = _position;
	transformations = std::vector<Transformation>{ _transformation };
}

DrawableType Drawable::GetDrawableType()
{
	return drawableType;
}

Vector2 Drawable::GetPosition()
{
	return position;
}

std::vector<Transformation> Drawable::GetTransformations()
{
	return transformations;
}

bool Drawable::NeedsToDraw()
{
	return *timer >= transformations.front().GetTime1() && *timer < transformations.back().GetTime2();
}

void Drawable::PushTransformation(Transformation _transformation)
{
	transformations.push_back(_transformation);
}

int* Drawable::GetTimer()
{
	return timer;
}