#pragma once
#include "../../../Sdk/DataTypes/Structs/Vector2.h"
#include "Transformation.h"

namespace ReplayEditor
{
	enum DrawableType
	{
		Drawable_HitObjectOsu,
		Drawable_HitObjectSliderOsu,
		Drawable_HitObjectSpinnerOsu
	};

	class Drawable
	{
		DrawableType drawableType;
		Vector2 position;
		Transformation transformation;
	public:
		Drawable(DrawableType _drawableType, Vector2 _position, Transformation _transformation = Transformation());
		DrawableType GetDrawableType();
		Vector2 GetPosition();
		Transformation GetTransformation();
		void SetTransformation(Transformation _transformation);
	};
}