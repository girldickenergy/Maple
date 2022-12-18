#pragma once

#include "imgui.h"

class Snowflake
{
public:
	ImVec2 Position;
	float Alpha;
	float Radius;
	int Offset;
	int Deviation;

	void UpdateParallax(ImVec2 currentCursorPosition, ImVec2 lastCursorPosition);
	void Update();
};
