#include "Drawable.h"

using namespace ReplayEditor;

Drawable::Drawable(DrawableType _drawableType, Vector2 _position, Transformation _transformation = Transformation())
{
	drawableType = _drawableType;
	position = _position;
	transformation = _transformation;
}

DrawableType Drawable::GetDrawableType()
{
	return drawableType;
}

Vector2 Drawable::GetPosition()
{
	return position;
}

Transformation ReplayEditor::Drawable::GetTransformation()
{
	return transformation;
}

void ReplayEditor::Drawable::SetTransformation(Transformation _transformation)
{
	transformation = _transformation;
}