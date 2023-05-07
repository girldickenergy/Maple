#include "HitObject.h"

bool HitObject::IsType(HitObjectType type)
{
	return (Type & type) > HitObjectType::None;
}

Vector2 HitObject::PositionAtLength(float length)
{
	if (IsType(HitObjectType::Slider))
	{
		if (SliderCurveSmoothLines.empty() || CumulativeLengths.empty())
			return Position;

		if (length == 0)
			return SliderCurveSmoothLines[0].first;

		double end = CumulativeLengths[CumulativeLengths.size() - 1];
		if (length >= end)
			return SliderCurveSmoothLines[SliderCurveSmoothLines.size() - 1].second;

		int i = 0;
		int num = CumulativeLengths.size() - 1;
		while (i <= num)
		{
			int num2 = i + (num - i >> 1);
			if (CumulativeLengths[num2] == length)
			{
				i = num2;
				break;
			}

			if (CumulativeLengths[num2] < length)
				i = num2 + 1;
			else
				num = num2 - 1;
		}
		i = ~i;

		if (i < 0)
			i = std::min(~i, static_cast<int>(CumulativeLengths.size()) - 1);

		const double lengthNext = CumulativeLengths[i];
		const double lengthPrevious = i == 0 ? 0 : CumulativeLengths[i - 1];

		Vector2 res = SliderCurveSmoothLines[i].first;

		if (lengthNext != lengthPrevious)
			res = res + (SliderCurveSmoothLines[i].second - SliderCurveSmoothLines[i].first) * static_cast<float>((length - lengthPrevious) / (lengthNext - lengthPrevious));

		return res;
	}

	return Position;
}

Vector2 HitObject::PositionAtTime(int time)
{
	if (IsType(HitObjectType::Slider))
	{
		if (SliderCurveSmoothLines.empty())
			return Position;

		if (time < StartTime)
			return Position;

		if (time > EndTime)
			return EndPosition;

		double num = static_cast<double>(time - StartTime) / (static_cast<double>(Duration) / static_cast<double>(SegmentCount));
		if (fmod(num, 2.0) >= 1.0)
			num = 1.0 - fmod(num, 1.0);
		else
			num = fmod(num, 1.0);

		const auto length = static_cast<float>(SpatialLength * num);

		return PositionAtLength(length);
	}

	return Position;
}

HitObject::HitObject(int count, HitObjectType type, int startTime, int endTime, Vector2 position, Vector2 endPosition, int segmentCount, int stackCount, double spatialLength, std::vector<Vector2> sliderCurvePoints, std::vector<std::pair<Vector2, Vector2>> sliderCurveSmoothLines, std::vector<double> cumulativeLengths, double velocity, std::vector<int> sliderScoreTimingPoints)
{
	IsHit = false;
	Is100 = false;
	Is50 = false;
	Count = count;
	Type = type;
	StartTime = startTime;
	EndTime = endTime;
	Duration = EndTime - StartTime;
	Position = position;
	EndPosition = endPosition;
	SegmentCount = segmentCount;
	StackCount = stackCount;
	SpatialLength = spatialLength;
	SliderCurvePoints = sliderCurvePoints;
	SliderCurveSmoothLines = sliderCurveSmoothLines;
	CumulativeLengths = cumulativeLengths;
	Velocity = velocity;
	SliderScoreTimingPoints = sliderScoreTimingPoints;
}

HitObject::HitObject()
{
	IsHit = false;
	Is100 = false;
	Is50 = false;
	IsNull = true;
}