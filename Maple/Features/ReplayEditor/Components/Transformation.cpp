#include "Transformation.h"

using namespace ReplayEditor;

Transformation::Transformation()
{
	transformationType = TransformationType::Empty;
}

Transformation::Transformation(TransformationType _transformationType, float _startFloat, float _endFloat, int _startTime, int _endTime)
{
	transformationType = _transformationType;
	startFloat = _startFloat;
	endFloat = _endFloat;
	startTime = _startTime;
	endTime = _endTime;
}

TransformationType Transformation::GetTransformationType()
{
	return transformationType;
}

float ReplayEditor::Transformation::GetStartFloat()
{
	return startFloat;
}

float ReplayEditor::Transformation::GetEndFloat()
{
	return endFloat;
}

int ReplayEditor::Transformation::GetStartTime()
{
	return startTime;
}

int ReplayEditor::Transformation::GetEndTime()
{
	return endTime;
}