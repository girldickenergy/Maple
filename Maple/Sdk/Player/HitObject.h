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
	int StackCount;
	double SpatialLength;
	std::vector<Vector2> SliderCurvePoints;
	std::vector<std::pair<Vector2, Vector2>> SliderCurveSmoothLines;
	std::vector<double> CumulativeLengths;
	double Velocity;
	std::vector<int> SliderScoreTimingPoints;

	// Replay Editor | TODO: make a new class and relocate this
	int Count;
	bool IsHit;
	bool Is100;
	bool Is50;

	bool IsType(HitObjectType type);
	Vector2 PositionAtLength(float length);
	Vector2 PositionAtTime(int time);

	HitObject(int count, HitObjectType type, int startTime, int endTime, Vector2 position, Vector2 endPosition, int segmentCount, 
		int stackCount, double spatialLength, std::vector<Vector2> sliderCurvePoints = {}, std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines = {}, 
		std::vector<double> cumulativeLengths = {}, double velocity = 0, std::vector<int> sliderScoreTimingPoints = {});
	HitObject();
};
