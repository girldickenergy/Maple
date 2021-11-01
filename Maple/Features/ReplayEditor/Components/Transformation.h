#pragma once

namespace ReplayEditor
{
	enum TransformationType
	{
		Fade,
		Scale
	};

	class Transformation
	{
		TransformationType transformationType;
		float startFloat;
		float endFloat;
		int time1;
		int time2;
	public:
		Transformation();
		Transformation(TransformationType _transformationType, float _startFloat, float _endFloat, int _time1, int _time2);

		TransformationType GetTransformationType();
		float GetStartFloat();
		float GetEndFloat();
		int GetTime1();
		int GetTime2();
	};
}