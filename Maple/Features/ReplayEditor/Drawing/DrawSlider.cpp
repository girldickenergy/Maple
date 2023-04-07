#include "DrawSlider.h"
#include "../../../Dependencies/clipper/clipper.hpp"
#include "../EditorGlobals.h"

void ReplayEditor::DrawSlider::CalculateCurves(SliderOsu* sliderOsu, float circleSize, bool hardRock)
{
	ClipperLib::ClipperOffset co;
	ClipperLib::Path p;
	ClipperLib::Paths solution;

	for (auto& [first, second] : sliderOsu->GetPoints())
	{
		if (hardRock)
		{
			first = Vector2(first.X, 384 - first.Y);
			second = Vector2(second.X, 384 - second.Y);
		}
		const Vector2 point1 = EditorGlobals::ConvertToPlayArea(first);
		const Vector2 point2 = EditorGlobals::ConvertToPlayArea(second);

		p << ClipperLib::IntPoint(point1.X, point1.Y) << ClipperLib::IntPoint(point2.X, point2.Y);
	}
	co.AddPath(p, ClipperLib::jtRound, ClipperLib::etOpenRound);
	co.Execute(solution, circleSize * sliderOsu->GetScale());

	for (auto& path : solution)
		for (const auto& point : path)
			sliderOsu->AddPoint(Vector2(point.X, point.Y));
}

void ReplayEditor::DrawSlider::Render(SliderOsu* sliderOsu, ImDrawList* drawList, float circleSize)
{
	if (sliderOsu->NeedsToDraw())
	{
		float sliderScale = circleSize * sliderOsu->GetScale();

		sliderOsu->DoTransformations();

		const std::vector<Vector2> points = sliderOsu->GetAllPoints();

		for (int i = 0; i < points.size() - 1; i++)
		{
			const auto point1 = points[i];
			const auto point2 = points[i + 1];
			drawList->AddLine(ImVec2(point1.X, point1.Y), ImVec2(point2.X, point2.Y),
				ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f),
					sliderOsu->GetOpacity())), 4.f);
		}
		drawList->AddLine(ImVec2(points.front().X, points.front().Y), ImVec2(points.back().X, points.back().Y),
			ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f),
				sliderOsu->GetOpacity())), 4.f);
	}
}
