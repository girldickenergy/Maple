#include "SliderOsu.h"

#include "../../../EditorGlobals.h"

ReplayEditor::SliderOsu::SliderOsu(HitObject* _internalHitObject, int _time, int _preempt, int* _timer, Vector2 _position, double _velocity, int _segmentCount, Transformation _transformation, std::vector<int> _sliderScoreTimingPoints, std::vector<double> _cumulativeLengths, std::vector<std::pair<Vector2, Vector2>> _points, std::vector<Vector2> _allPoints)
	: OsuDrawable(DrawableType::Drawable_HitObjectSliderOsu, _timer, _position, _transformation)
{
	internalHitObject = _internalHitObject;

	time = _time;
	preempt = _preempt;
	points = _points;
	allPoints = _allPoints;
	velocity = _velocity;
	segmentCount = _segmentCount;
	cumulativeLengths = _cumulativeLengths;
	sliderScoreTimingPoints = _sliderScoreTimingPoints;
}

void ReplayEditor::SliderOsu::SetTime(int _time)
{
	time = _time;
}

int ReplayEditor::SliderOsu::GetTime()
{
	return time;
}

void ReplayEditor::SliderOsu::SetPreempt(int _preempt)
{
	preempt = _preempt;
}

int ReplayEditor::SliderOsu::GetPreempt()
{
	return preempt;
}

void ReplayEditor::SliderOsu::SetPoints(std::vector<std::pair<Vector2, Vector2>> _points)
{
	points = _points;
}

std::vector<std::pair<Vector2, Vector2>> ReplayEditor::SliderOsu::GetPoints()
{
	return points;
}

void ReplayEditor::SliderOsu::SetAllPoints(std::vector<Vector2> _allPoints)
{
	allPoints = _allPoints;
}

void ReplayEditor::SliderOsu::AddPoint(Vector2 _point)
{
	allPoints.push_back(_point);
}

std::vector<Vector2> ReplayEditor::SliderOsu::GetAllPoints()
{
	return allPoints;
}

void ReplayEditor::SliderOsu::InitializeSliderBall()
{
	sliderBallOsu = new SliderBallOsu(time, preempt, GetTimer(), GetPosition());
	
	for (auto const transformation : GetTransformations())
		sliderBallOsu->PushTransformation(transformation);

	auto startTime = time;
	Vector2 vector1 = Vector2();
	Vector2 vector2 = Vector2();
	for (int i = 0; i < segmentCount; i++)
	{
		const bool flag = i % 2 == 1;
		const int startingIndex = (flag ? (points.size() - 1) : 0);
		const int stoppingIndex = (flag ? (-1) : points.size());
		const int increment = (flag ? (-1) : 1);

		for (int j = startingIndex; j != stoppingIndex; j += increment)
		{
			const auto line = points[j];

			const float initialDuration = static_cast<float>(cumulativeLengths[j] - ((j == 0) ? 0.0f : cumulativeLengths[j - 1]));
			if (flag)
			{
				vector1 = line.second;
				vector2 = line.first;
			}
			else
			{
				vector1 = line.first;
				vector2 = line.second;
			}

			const double transformationDuration = static_cast<double>(1000.f * initialDuration) / velocity;
			Transformation transformation = Transformation(TransformationType::Move, vector1, vector2, startTime, startTime + transformationDuration);
			sliderBallOsu->PushTransformation(transformation);

			startTime += transformationDuration;	
		}
	}
}

void ReplayEditor::SliderOsu::InitializeSliderTicks()
{
	for (auto const& time : sliderScoreTimingPoints)
	{
		if (time == GetTime() || time == internalHitObject->EndTime)
			continue;

		auto position = internalHitObject->PositionAtTime(time);
		
		auto sliderTick = new SliderTickOsu(time, preempt, GetTimer(), EditorGlobals::ConvertToPlayArea(position));

		for (auto const transformation : GetTransformations())
			sliderTick->PushTransformation(transformation);

		sliderTicks.push_back(sliderTick);
	}
}

ReplayEditor::SliderBallOsu* ReplayEditor::SliderOsu::GetSliderBall()
{
	return sliderBallOsu;
}

std::vector<ReplayEditor::SliderTickOsu*> ReplayEditor::SliderOsu::GetSliderTicks()
{
	return sliderTicks;
}
