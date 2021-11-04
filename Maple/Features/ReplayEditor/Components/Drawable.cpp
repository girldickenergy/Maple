#include "Drawable.h"
#include <algorithm>
#include <string>

using namespace ReplayEditor;

Drawable::Drawable(DrawableType _drawableType, int* _timer, Vector2 _position, Transformation _transformation)
{
	drawableType = _drawableType;
	timer = _timer;
	position = _position;
	transformations = std::vector<Transformation>{ _transformation };

	opacity = 1;
	scale = 1;
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

std::tuple<int, int> Drawable::GetDrawingTimes()
{
	int earliestStartTime = 0, latestEndTime = 0;
	for (auto& transformation : transformations)
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

	return *timer >= std::get<0>(drawingTimes) && *timer < std::get<1>(drawingTimes);
}

void Drawable::PushTransformation(Transformation _transformation)
{
	transformations.push_back(_transformation);
}

int* Drawable::GetTimer()
{
	return timer;
}

void Drawable::ApplyTransformation(Transformation _transformation)
{
	int duration = _transformation.GetEndTime() - _transformation.GetStartTime();
	float t = *timer - _transformation.GetStartTime() / duration;
	t = std::clamp(t, 0.f, 1.f);

	printf("[x] transformation -> end: %i start: %i type: %i\n", _transformation.GetEndTime(), _transformation.GetStartTime(), _transformation.GetTransformationType());
	switch (_transformation.GetTransformationType())
	{
	case TransformationType::Fade:
	{
		float lerpedFloat = std::lerp(_transformation.GetStartFloat(), _transformation.GetEndFloat(), t);
		printf("[x] lerpedFloat -> %f\n", lerpedFloat);
		opacity = lerpedFloat;
	}
	break;
	case TransformationType::Scale:
	{
		float lerpedFloat = std::lerp(_transformation.GetStartFloat(), _transformation.GetEndFloat(), t);
		printf("[x] lerpedFloat -> %f\n", lerpedFloat);
		scale = lerpedFloat;
	}
	break;
	}
}

void Drawable::DoTransformations()
{
	//if (NeedsToDraw) <-- We'll probably call this function in the UI thread anyways now that I think about it.
	for (auto& transformation : transformations)
	{
		if (transformation.GetTransformationType() != TransformationType::Empty)
			ApplyTransformation(transformation);
	}
}

float Drawable::GetOpacity()
{
	return opacity;
}

float Drawable::GetScale()
{
	return scale;
}
