#include "SliderOsu.h"

#include "../../../EditorGlobals.h"
#include "../../../Editor.h"

ReplayEditor::SliderOsu::SliderOsu(HitObject* _internalHitObject, int _index, int _time, int _preempt, int* _timer, Vector2 _position, double _velocity, int _segmentCount, Transformation _transformation, std::vector<int> _sliderScoreTimingPoints, std::vector<double> _cumulativeLengths, std::vector<std::pair<Vector2, Vector2>> _points, std::vector<Vector2> _allPoints)
	: OsuDrawable(DrawableType::Drawable_HitObjectSliderOsu, _timer, _time, _position, _transformation, _index)
{
	internalHitObject = _internalHitObject;

	preempt = _preempt;
	points = _points;
	allPoints = _allPoints;
	velocity = _velocity;
	segmentCount = _segmentCount;
	cumulativeLengths = _cumulativeLengths;
	sliderScoreTimingPoints = _sliderScoreTimingPoints;

	_sliderTicksHit = 0;
	_sliderTicksMissed = 0;

	SetHitObjectScoring(HitObjectScoring::ThreeHundred);
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
	sliderBallOsu = new SliderBallOsu(GetTime(), preempt, GetTimer(), GetPosition());

	for (auto const transformation : GetTransformations())
		sliderBallOsu->PushTransformation(transformation);

	auto startTime = GetTime();
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
	sliderTicks.clear();

	auto radius = Editor::Get().GetHitObjectManagerWrapper().GetHitObjectRadius();
	for (auto it = sliderScoreTimingPoints.begin(); it < sliderScoreTimingPoints.end() - 1; ++it)
	{
		auto time = *it;
		if (time == GetTime() || time == internalHitObject->EndTime)
			continue;

		auto position = internalHitObject->PositionAtTime(time);

		if (position.DistanceSquared(internalHitObject->PositionAtTime(*(sliderScoreTimingPoints.end() - 1))) <= radius * radius ||
			position.DistanceSquared(GetPosition()) <= radius * radius)
			continue;

		auto sliderTick = new SliderTickOsu(time, preempt, GetTimer(), EditorGlobals::ConvertToPlayArea(position));

		AnalyzeSliderTick(time, sliderTick);

		for (auto const transformation : GetTransformations())
			sliderTick->PushTransformation(transformation);

		sliderTicks.push_back(sliderTick);
	}

	for (auto const& tick : sliderTicks)
		if (tick->GetHitObjectScoring() == HitObjectScoring::Miss)
		{
			SetHitObjectScoring(HitObjectScoring::Miss);
			return;
		}
		else if (tick->GetHitObjectScoring() == HitObjectScoring::OneHundred)
		{
			SetHitObjectScoring(HitObjectScoring::OneHundred);
			return;
		}
	SetHitObjectScoring(HitObjectScoring::ThreeHundred);
}

void ReplayEditor::SliderOsu::AnalyzeSliderTick(int time, SliderTickOsu* sliderTickOsu)
{
	bool allowable = false;
	auto sliderBall = GetSliderBall();
	auto sliderBallTransformation = sliderBall->GetTransformations();
	auto replayHandler = Editor::Get().GetReplayHandler();
	auto radius = Editor::Get().GetHitObjectManagerWrapper().GetHitObjectRadius() * 2.4f;

	Transformation* t = nullptr;
	for (auto& transformation : sliderBallTransformation)
	{
		if (transformation.GetStartTime() <= time && transformation.GetEndTime() >= time)
		{
			t = &transformation;
			break;
		}
	}

	if (t == nullptr && time > sliderBallTransformation[sliderBallTransformation.size() - 1].GetEndTime())
		t = &sliderBallTransformation[sliderBallTransformation.size() - 1];

	if (t != nullptr)
	{
		Vector2 position;
		if (t->GetEndTime() == t->GetStartTime())
			position = t->GetEndPosition();
		else
			position = t->GetStartPosition() +
			(t->GetEndPosition() - t->GetStartPosition()) *
			(1 - static_cast<float>(t->GetEndTime() - time) / (t->GetEndTime() - t->GetStartTime()));

		allowable = replayHandler.GetMousePositionAtTime(time).DistanceSquared(position) < radius * radius;
	}

	if (allowable && GetTime() <= time)
	{
		_sliderTicksHit++;
		sliderTickOsu->SetHitObjectScoring(HitObjectScoring::ThreeHundred);
	}
	else
	{
		_sliderTicksMissed++;

		if (_sliderTicksHit + _sliderTicksMissed == sliderScoreTimingPoints.size())
			sliderTickOsu->SetHitObjectScoring(HitObjectScoring::OneHundred);
		else
			sliderTickOsu->SetHitObjectScoring(HitObjectScoring::Miss);
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