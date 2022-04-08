#pragma once

#include <imgui.h>
#include <vector>

class Snowflake
{
public:
	ImVec2 Position;
	float Alpha;
	float Radius;
	int Offset;
	int Deviation;

	void UpdateParallax(ImVec2 currentCursorPosition, ImVec2 lastCursorPosition)
	{
		Offset += (int)((currentCursorPosition.x - lastCursorPosition.x) * 10.f);
	}

	void Update()
	{
		float frameRatio = (ImGui::GetIO().DeltaTime * 1000.f) / (1000.f / 60.f);

		if (Offset != 0 || Deviation != 0)
		{
			Offset += Deviation;
			Position.x = Position.x + (float)Offset / 5000.f * frameRatio;
			Position.y = Position.y + frameRatio;
		}
	}
};

class SnowVisualisation
{
	static inline float spawnTimer = 0.f;
	static inline ImVec2 currentCursorPosition = ImVec2(0, 0);
	static inline ImVec2 lastCursorPosition = ImVec2(0, 0);
	static inline std::vector<Snowflake> snowflakes;
public:
	static void Draw();
};