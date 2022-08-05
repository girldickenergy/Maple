#pragma once

#include <vector>

#include "Math/Vector2.h"
#include "Osu/HitObjects/HitObjectType.h"

class HitObject
{
public:
	bool IsNull = false;
	HitObjectType Type;
	int StartTime;
	int EndTime;
	int Duration;
	Vector2 Position;
	Vector2 EndPosition;
	int SegmentCount;
	double SpatialLength;
	std::vector<Vector2> SliderCurvePoints;
	std::vector<std::pair<Vector2, Vector2>> SliderCurveSmoothLines;
	std::vector<double> CumulativeLengths;

	bool IsType(HitObjectType type);
	Vector2 PositionAtLength(float length);
	Vector2 PositionAtTime(int time);

	HitObject(HitObjectType type, int startTime, int endTime, Vector2 position, Vector2 endPosition, int segmentCount, double spatialLength, std::vector<Vector2> sliderCurvePoints = {}, std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines = {}, std::vector<double> cumulativeLengths = {});
	HitObject();
};
