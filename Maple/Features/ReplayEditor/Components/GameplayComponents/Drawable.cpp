#include "Drawable.h"
#include <algorithm>
#include <string>

using namespace ReplayEditor;

Drawable::Drawable(DrawableType drawableType, int* timer, Vector2 position, Transformation transformation, int index)
{
	_drawableType = drawableType;
	_timer = timer;
	_position = position;
	_transformations = std::vector<Transformation>{ transformation };

	_opacity = 1;
	_scale = 1;
	_index = index;
}

std::tuple<int, int> Drawable::GetDrawingTimes()
{
	int earliestStartTime = 0, latestEndTime = 0;
	for (auto& transformation : _transformations)
	{
		if (transformation.GetTransformationType() != TransformationType::Empty) {
			if (earliestStartTime > transformation.GetStartTime() || earliestStartTime == 0)
				earliestStartTime = transformation.GetStartTime();
			if (latestEndTime < transformation.GetEndTime() || latestEndTime == 0)
				latestEndTime = transformation.GetEndTime();
		}
	}
	return std::make_tuple(earliestStartTime, latestEndTime);
}

bool Drawable::NeedsToDraw()
{
	std::tuple<int, int> drawingTimes = GetDrawingTimes();

	return *_timer >= std::get<0>(drawingTimes) && *_timer < std::get<1>(drawingTimes);
}

void Drawable::PushTransformation(Transformation transformation)
{
	_transformations.push_back(transformation);
}

void Drawable::ApplyTransformation(Transformation transformation)
{
	if (transformation.GetStartTime() <= *_timer) {
		int duration = transformation.GetEndTime() - transformation.GetStartTime();
		float t = (static_cast<float>(*_timer) - static_cast<float>(transformation.GetStartTime())) / static_cast<float>(duration);
		t = std::clamp(t, 0.f, 1.f);

		switch (transformation.GetTransformationType())
		{
			case TransformationType::Fade:
			{
				float lerpedFloat = std::lerp(transformation.GetStartFloat(), transformation.GetEndFloat(), t);
				_opacity = lerpedFloat;
			}
			break;
			case TransformationType::Scale:
			{
				float lerpedFloat = std::lerp(transformation.GetStartFloat(), transformation.GetEndFloat(), t);
				_scale = lerpedFloat;
			}
			break;
			case TransformationType::Move:
			{
				float lerpedX = std::lerp(transformation.GetStartPosition().X, transformation.GetEndPosition().X, t);
				float lerpedY = std::lerp(transformation.GetStartPosition().Y, transformation.GetEndPosition().Y, t);
				_position = Vector2(lerpedX, lerpedY);
			}
			break;
		}
	}
}

void Drawable::DoTransformations()
{
	for (auto& transformation : _transformations)
	{
		if (transformation.GetTransformationType() != TransformationType::Empty)
			ApplyTransformation(transformation);
	}
}

float Drawable::GetOpacity()
{
	return _opacity;
}

float Drawable::GetScale()
{
	return _scale;
}

int ReplayEditor::Drawable::GetIndex()
{
	return _index;
}

DrawableType Drawable::GetDrawableType()
{
	return _drawableType;
}

Vector2 Drawable::GetPosition()
{
	return _position;
}

std::vector<Transformation> Drawable::GetTransformations()
{
	return _transformations;
}

int* Drawable::GetTimer()
{
	return _timer;
}

void Drawable::SetColor(ImU32 color)
{
	_color = color;
}

ImU32 Drawable::GetColor()
{
	return _color;
}