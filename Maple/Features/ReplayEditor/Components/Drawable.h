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
		float opacity;
		float scale;
	public:
		Drawable(DrawableType _drawableType, int* _timer, Vector2 _position, Transformation _transformation = Transformation());
		DrawableType GetDrawableType();
		Vector2 GetPosition();
		std::vector<Transformation> GetTransformations();
		std::tuple<int, int> GetDrawingTimes();
		bool NeedsToDraw();
		int* GetTimer();
		void PushTransformation(Transformation _transformation);
		void ApplyTransformation(Transformation _transformation);
		void DoTransformations();
		float GetOpacity();
		float GetScale();
	};
}