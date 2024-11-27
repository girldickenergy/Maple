#pragma once

#include <vector>

#include "Math/Vector2.h"
#include "Osu/HitObjects/HitObjectType.h"
#include "Osu/HitObjects/HitSoundType.h"

class HitObject
{
public:
	bool IsNull = false;
	HitObjectType Type;
	HitSoundType HitSound;
	int StartTime;
	int EndTime;
	int Duration;
	Vector2 Position;
	Vector2 EndPosition;
	int SegmentCount;
	double SpatialLength;
	double MinHitDelay;
	int RotationRequirement;
	std::vector<Vector2> SliderCurvePoints;
	std::vector<std::pair<Vector2, Vector2>> SliderCurveSmoothLines;
	std::vector<double> CumulativeLengths;

	bool IsType(HitObjectType type);
	bool HasHitSound(HitSoundType hitSound);
	Vector2 PositionAtLength(float length);
	Vector2 PositionAtTime(int time);

	HitObject(HitObjectType type, HitSoundType hitSound, int startTime, int endTime, Vector2 position, Vector2 endPosition, int segmentCount, double spatialLength, double minHitDelay = 0.0, int rotationRequirement = 0, const std::vector<Vector2>& sliderCurvePoints = {}, const std::vector<std::pair<Vector2, Vector2>>& sliderCurveSmoothLines = {}, const std::vector<double>& cumulativeLengths = {});
	HitObject();
};
