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

	_opacity = 0.f;
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
	return std::make_tuple(earliestStartTime + 1, latestEndTime);
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

void Drawable::DoTransformations()
{
	bool hasFade = false;
	bool hasScale = false;
	bool hasMove = false;

	bool hasFuture = false;
	bool hasPast = false;
	bool shouldDraw = false;

	for (auto& transformation : _transformations)
	{
		if (transformation.GetTransformationType() == TransformationType::Empty)
			continue;

		if (transformation.GetStartTime() >= *_timer || transformation.GetEndTime() > *_timer)
		{
			hasFuture = true;
			if (transformation.GetStartTime() > *_timer)
				continue;
		}

		if (transformation.GetEndTime() <= *_timer)
		{
			hasPast = true;
			if (transformation.GetEndTime() < *_timer)
				continue;
		}

		shouldDraw = true;

		switch (transformation.GetTransformationType())
		{
			case TransformationType::Fade:
				if (hasFade) 
					break;
				ApplyTransformation(transformation);
				hasFade = true;
				break;
			case TransformationType::Scale:
				if (hasScale)
					break;
				ApplyTransformation(transformation);
				hasScale = true;
				break;
			case TransformationType::Move:
				if (hasMove)
					break;
				ApplyTransformation(transformation);
				hasMove = true;
				break;
		}
	}

	if ((!hasFuture && !shouldDraw) || (!(hasFuture && hasPast) && !shouldDraw))
		return;

	// Apply past transformations
	for (auto it = _transformations.end() - 1; it != _transformations.begin(); --it)
	{
		auto transformation = *it;
		if (transformation.GetEndTime() >= *_timer)
			continue;

		switch (transformation.GetTransformationType())
		{
			case TransformationType::Fade:
				if (hasFade)
					break;
				_opacity = transformation.GetEndFloat();
				hasFade = true;
				break;
			case TransformationType::Scale:
				if (hasScale)
					break;
				_scale = transformation.GetEndFloat();
				hasScale = true;
				break;
			case TransformationType::Move:
				if (hasMove)
					break;
				_position = transformation.GetEndPosition();
				hasMove = true;
				break;
		}
	}

	// Apply future transformations
	if (hasFuture)
	{
		for (auto& transformation : _transformations)
		{
			if (transformation.GetStartTime() < *_timer)
				continue;

			switch (transformation.GetTransformationType())
			{
				case TransformationType::Fade:
					if (hasFade)
						break;
					_opacity = transformation.GetStartFloat();
					hasFade = true;
					break;
				case TransformationType::Scale:
					if (hasScale)
						break;
					_scale = transformation.GetStartFloat();
					hasScale = true;
					break;
				case TransformationType::Move:
					if (hasMove)
						break;
					_position = transformation.GetStartPosition();
					hasMove = true;
					break;
			}
		}
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