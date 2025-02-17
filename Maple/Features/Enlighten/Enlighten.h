#pragma once

#include <tuple>
#include <vector>

#include "imgui.h"

class Enlighten
{
	static inline int preEmpt;

	static inline float preemtiveDotRadius;
	static inline std::vector<std::tuple<ImVec2, int>> preemptiveDots;
	static void initializePreemptiveDots();
public:
	static void Initialize();

	static void Render();
};
