#pragma once

#include <vector>

#include "Snowflake.h"

class SnowVisualisation
{
	static inline float spawnTimer = 0.f;
	static inline ImVec2 currentCursorPosition = ImVec2(0, 0);
	static inline ImVec2 lastCursorPosition = ImVec2(0, 0);
	static inline std::vector<Snowflake> snowflakes;
public:
	static void Render();
};