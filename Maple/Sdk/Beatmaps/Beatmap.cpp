#include "Beatmap.h"

Beatmap::Beatmap(void* _beatmapOsuPointer)
{
	RawBeatmapBase = Vanilla::Explorer["osu.GameplayElements.Beatmaps.BeatmapBase"];
	RawBeatmap	   = Vanilla::Explorer["osu.GameplayElements.Beatmaps.Beatmap"];

	difficultyApproachRateField = RawBeatmapBase["DifficultyApproachRate"].Field;
	difficultyCircleSizeField = RawBeatmapBase["DifficultyCircleSize"].Field;
	difficultyOverallField = RawBeatmapBase["DifficultyOverall"].Field;
	displayTitleFullField = RawBeatmap["DisplayTitleFull"].Field;

	beatmapOsuPointer = _beatmapOsuPointer;

	Update();
}

void Beatmap::Update()
{
	approachRate	  = *static_cast<float*>(difficultyApproachRateField.GetAddress(beatmapOsuPointer));
	circleSize		  = *static_cast<float*>(difficultyCircleSizeField.GetAddress(beatmapOsuPointer));
	overallDifficulty = *static_cast<float*>(difficultyOverallField.GetAddress(beatmapOsuPointer));
	displayTitleFull  = (*static_cast<COMString*>(displayTitleFullField.GetAddress(beatmapOsuPointer))).Data();
}

float Beatmap::GetApproachRate()
{
	return approachRate;
}

float Beatmap::GetCircleSize()
{
	return circleSize;
}

float Beatmap::GetOverallDifficulty()
{
	return overallDifficulty;
}

void* Beatmap::GetBeatmapOsuPointer()
{
	return beatmapOsuPointer;
}

std::wstring Beatmap::GetDisplayTitleFull()
{
	return std::wstring(displayTitleFull.begin(), displayTitleFull.end());
}