#include "Snowflake.h"

void Snowflake::UpdateParallax(ImVec2 currentCursorPosition, ImVec2 lastCursorPosition)
{
	Offset += static_cast<int>((currentCursorPosition.x - lastCursorPosition.x) * 10.f);
}

void Snowflake::Update()
{
	const float frameRatio = (ImGui::GetIO().DeltaTime * 1000.f) / (1000.f / 60.f);

	if (Offset != 0 || Deviation != 0)
	{
		Offset += Deviation;
		Position.x = Position.x + static_cast<float>(Offset) / 5000.f * frameRatio;
		Position.y = Position.y + frameRatio;
	}
}
