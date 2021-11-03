#include "Transformation.h"

using namespace ReplayEditor;

Transformation::Transformation() { }

Transformation::Transformation(TransformationType _transformationType, float _startFloat, float _endFloat, int _time1, int _time2)
{
	transformationType = _transformationType;
	startFloat = _startFloat;
	endFloat = _endFloat;
	time1 = _time1;
	time2 = _time2;
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

int ReplayEditor::Transformation::GetTime1()
{
	return time1;
}

int ReplayEditor::Transformation::GetTime2()
{
	return time2;
}