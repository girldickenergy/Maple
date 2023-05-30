#pragma once
#include "OsuDrawable.h"
#include "Math/Vector2.h"

#include "SliderBallOsu.h"
#include "SliderTickOsu.h"
#include "../../../../../Sdk/Player/HitObject.h"

namespace ReplayEditor
{
	class SliderOsu : public OsuDrawable
	{
		using OsuDrawable::OsuDrawable;

		int preempt;
		double velocity;
		int segmentCount;
		int _sliderTicksHit;
		int _sliderTicksMissed;
		std::vector<std::pair<Vector2, Vector2>> points;
		std::vector<Vector2> allPoints;
		std::vector<int> sliderScoreTimingPoints;
		std::vector<double> cumulativeLengths;

		ReplayEditor::SliderBallOsu* sliderBallOsu;
		std::vector<ReplayEditor::SliderTickOsu*> sliderTicks;

		HitObject* internalHitObject;
	public:
		SliderOsu(HitObject* _internalHitObject, int _index, int _time, int _preempt, int* _timer, Vector2 _position, double _velocity, int _segmentCount, Transformation _transformation, std::vector<int> _sliderScoreTimingPoints, std::vector<double> _cumulativeLengths, std::vector<std::pair<Vector2, Vector2>> _points, std::vector<Vector2> _allPoints = std::vector<Vector2>());
		void SetPreempt(int _preempt);
		int GetPreempt();
		void SetPoints(std::vector<std::pair<Vector2, Vector2>> _points);
		std::vector<std::pair<Vector2, Vector2>> GetPoints();
		void SetAllPoints(std::vector<Vector2> _allPoints);
		void AddPoint(Vector2 _point);
		std::vector<Vector2> GetAllPoints();

		void InitializeSliderBall();
		void InitializeSliderTicks();
		void AnalyzeSliderTick(int time, SliderTickOsu* sliderTickOsu);

		ReplayEditor::SliderBallOsu* GetSliderBall();
		std::vector<ReplayEditor::SliderTickOsu*> GetSliderTicks();
	};
}
