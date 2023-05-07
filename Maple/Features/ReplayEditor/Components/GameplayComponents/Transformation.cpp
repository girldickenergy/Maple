#include "Transformation.h"

using namespace ReplayEditor;

Transformation::Transformation()
{
	_transformationType = TransformationType::Empty;
}

Transformation::Transformation(TransformationType transformationType, float startFloat, float endFloat, int startTime, int endTime, EasingType easingType)
{
	_transformationType = transformationType;
	_start = Vector2(startFloat, 0.f);
	_end = Vector2(endFloat, 0.f);
	_startTime = startTime;
	_endTime = endTime;
	_easingType = easingType;
}

Transformation::Transformation(TransformationType transformationType, Vector2 startPosition, Vector2 endPosition, int startTime, int endTime, EasingType easingType)
{
	_transformationType = transformationType;
	_start = startPosition;
	_end = endPosition;
	_startTime = startTime;
	_endTime = endTime;
	_easingType = easingType;
}

TransformationType Transformation::GetTransformationType()
{
	return _transformationType;
}

float ReplayEditor::Transformation::GetStartFloat()
{
	return _start.X;
}

float ReplayEditor::Transformation::GetEndFloat()
{
	return _end.X;
}

Vector2 ReplayEditor::Transformation::GetStartPosition()
{
	return _start;
}

Vector2 ReplayEditor::Transformation::GetEndPosition()
{
	return _end;
}

int ReplayEditor::Transformation::GetStartTime()
{
	return _startTime;
}

int ReplayEditor::Transformation::GetEndTime()
{
	return _endTime;
}

void ReplayEditor::Transformation::SetEndTime(int endTime)
{
	_endTime = endTime;
}

EasingType ReplayEditor::Transformation::GetEasingType()
{
	return _easingType;
}

void ReplayEditor::Transformation::SetEasingType(EasingType easingType)
{
	_easingType = easingType;
}