#include "SnowVisualisation.h"

int randInt(int min, int max)
{
	return min + rand() / (RAND_MAX / (max - min));
}

float randFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void SnowVisualisation::Draw()
{
	spawnTimer += ImGui::GetIO().DeltaTime * 1000.f;
	currentCursorPosition = ImGui::GetIO().MousePos;

	if (snowflakes.size() > 0)
	{
		if (lastCursorPosition.x != 0 && lastCursorPosition.y != 0 && lastCursorPosition.x != currentCursorPosition.x && lastCursorPosition.y != currentCursorPosition.y)
			for (auto& snowflake : snowflakes)
				snowflake.UpdateParallax(currentCursorPosition, lastCursorPosition);

		lastCursorPosition = currentCursorPosition;
	}

	if (spawnTimer >= 50.f && randFloat(0.f, 1.f) > 0.5f)
	{
		Snowflake snowflake = Snowflake();
		snowflake.Position = ImVec2(randFloat(0.f, ImGui::GetIO().DisplaySize.x), -50.f);
		snowflake.Alpha = randFloat(0.f, 0.8f);
		snowflake.Radius = randFloat(2.f, 7.5f);
		snowflake.Offset = randInt(-500, 500);
		snowflake.Deviation = randInt(-2, 2);

		snowflakes.push_back(snowflake);

		spawnTimer = 0;
	}

	for (auto& snowflake : snowflakes)
	{
		snowflake.Update();

		ImGui::GetBackgroundDrawList()->AddCircleFilled(snowflake.Position, snowflake.Radius, ImColor(255, 255, 255, (int)(snowflake.Alpha * 255.0)), 16);
	}

    snowflakes.erase(std::remove_if(snowflakes.begin(), snowflakes.end(), [](Snowflake& snowflake)
    {
        return snowflake.Position.y - snowflake.Radius > ImGui::GetIO().DisplaySize.y;
    }), snowflakes.end());
}
