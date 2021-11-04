#pragma once
#include <iostream>
#include <cmath>

namespace ReplayEditor
{
	enum TransformationType : int
	{
		Empty,
		Fade,
		Scale
	};

	class Transformation
	{
		TransformationType transformationType;
		float startFloat;
		float endFloat;
		int startTime;
		int endTime;
	public:
		Transformation();
		Transformation(TransformationType _transformationType, float _startFloat, float _endFloat, int _time1, int _time2);
		TransformationType GetTransformationType();
		float GetStartFloat();
		float GetEndFloat();
		int GetStartTime();
		int GetEndTime();
	};
}