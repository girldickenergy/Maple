#pragma once
#include "../../../Sdk/DataTypes/Structs/Vector2.h"
#include "Transformation.h"
#include <vector>

namespace ReplayEditor
{
	enum DrawableType
	{
		Drawable_ApproachCircle,
		Drawable_HitObjectOsu,
		Drawable_HitObjectSliderOsu,
		Drawable_HitObjectSpinnerOsu
	};

	class Drawable
	{
		DrawableType drawableType;
		Vector2 position;
		std::vector<Transformation> transformations;
		int* timer;
	public:
		Drawable(DrawableType _drawableType, int* _timer, Vector2 _position, Transformation _transformation = Transformation());
		DrawableType GetDrawableType();
		Vector2 GetPosition();
		std::vector<Transformation> GetTransformations();
		bool NeedsToDraw();
		void PushTransformation(Transformation _transformation);
		int* GetTimer();
	};
}