#include "SliderOsu.h"

ReplayEditor::SliderOsu::SliderOsu(int _time, int _preempt, int* _timer, Vector2 _position, Transformation _transformation, std::vector<std::pair<Vector2, Vector2>> _points, std::vector<Vector2> _allPoints)
	: Drawable(DrawableType::Drawable_HitObjectSliderOsu, _timer, _position, _transformation)
{
	time = _time;
	preempt = _preempt;
	points = _points;
	allPoints = _allPoints;
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