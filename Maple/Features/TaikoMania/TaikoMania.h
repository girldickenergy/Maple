#pragma once

#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "TaikoManiaStage.h"

class TaikoMania
{
	static constexpr int SCROLL_DURATION = 750;

	static inline std::vector<TaikoManiaStage> stages;

	static inline ImRect clientRect;
	static inline ImRect playfieldRect;
	static inline int beatmapDuration;

	static void RenderPlayfield();
	static void RenderObjects(int time);
	static void RenderStatistics(int time);
public:
	static void Initialize();
	static void Render();
};
