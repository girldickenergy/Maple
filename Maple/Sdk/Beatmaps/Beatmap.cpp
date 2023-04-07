#include "Beatmap.h"

Beatmap::Beatmap()
{ }

Beatmap::Beatmap(uintptr_t beatmapOsuPointer)
{
	_beatmapOsuPointer = beatmapOsuPointer;
}

void Beatmap::Update()
{
	_approachRate = *reinterpret_cast<float*>(_beatmapOsuPointer + BEATMAP_APPROACHRATE_OFFSET);
	_circleSize = *reinterpret_cast<float*>(_beatmapOsuPointer + BEATMAP_CIRCLESIZE_OFFSET);
	_overallDifficulty = *reinterpret_cast<float*>(_beatmapOsuPointer + BEATMAP_OVERALLDIFFICULTY_OFFSET);

	const uintptr_t displayTitleFullPointer = _beatmapOsuPointer + BEATMAP_DISPLAYTITLEFULL_OFFSET;

	const int displayTitleFullLength = *reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(displayTitleFullPointer) + 0x04);
	auto title = std::string();

	for (int i = 0; i < displayTitleFullLength; i++)
		title += *reinterpret_cast<char*>(*reinterpret_cast<uintptr_t*>(displayTitleFullPointer) + 0x08 + i * 2);

	_displayTitleFull = title;
}

float Beatmap::GetApproachRate()
{
	return _approachRate;
}

float Beatmap::GetCircleSize()
{
	return _circleSize;
}

float Beatmap::GetOverallDifficulty()
{
	return _overallDifficulty;
}

uintptr_t Beatmap::GetBeatmapOsuPointer()
{
	return _beatmapOsuPointer;
}

std::string Beatmap::GetDisplayTitleFull()
{
	return _displayTitleFull;
}