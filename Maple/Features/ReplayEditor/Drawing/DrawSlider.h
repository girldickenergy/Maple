#pragma once
#include <imgui.h>
#include "../Components/GameplayComponents/Osu/SliderOsu.h"

namespace ReplayEditor
{
	class DrawSlider
	{
	public:
		/// @brief Calculates the offset curves and outputs them to the _points field in SliderOsu*.
		/// @param sliderOsu The pointer to the slider that the calculation is applied to.
		/// @param circleSize The circle size used for calculation.
		/// @param hardRock HardRock switch used for calculation.
		static void CalculateCurves(SliderOsu* sliderOsu, float circleSize, bool hardRock);

		/// @brief Renders the specified slider to screen.
		/// @param sliderOsu The pointer to the slider that will be drawn to screen.
		/// @param drawList The pointer to the ImGui DrawList.
		/// @param circleSize The circle size used for drawing.
		static void Render(SliderOsu* sliderOsu, ImDrawList* drawList, float circleSize);
	};
}